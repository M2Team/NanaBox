#include "pch.h"
#include "VirtualMachineCapture.h"
#include <Mile.Helpers.CppBase.h>
#include <audioclient.h>
#include <audioclientactivationparams.h>
#include <mmdeviceapi.h>
#include <d3d11.h>
#include <dwmapi.h>
#include <mfapi.h>
#include <shcore.h>
#include <windows.graphics.capture.interop.h>
#include <windows.graphics.directx.direct3d11.interop.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Media.Core.h>
#include <winrt/Windows.Media.MediaProperties.h>
#include <winrt/Windows.Media.Transcoding.h>
#include <winrt/Windows.Storage.h>
#include <algorithm>
#include <cstring>
#include <mutex>
#include <vector>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "mmdevapi.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "shcore.lib")
#pragma comment(lib, "windowsapp.lib")
namespace winrt
{
    using Windows::Foundation::TimeSpan;
    using Windows::Graphics::Capture::GraphicsCaptureItem;
    using Windows::Graphics::Capture::Direct3D11CaptureFrame;
    using Windows::Graphics::Capture::Direct3D11CaptureFramePool;
    using Windows::Graphics::Capture::GraphicsCaptureSession;
    using Windows::Graphics::DirectX::DirectXPixelFormat;
    using Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice;
    using Windows::Graphics::Imaging::BitmapPixelFormat;
    using Windows::Graphics::Imaging::BitmapEncoder;
    using Windows::Graphics::Imaging::SoftwareBitmap;
    using Windows::Media::Core::MediaStreamSource;
    using Windows::Media::Core::MediaStreamSample;
    using Windows::Media::Core::VideoStreamDescriptor;
    using Windows::Media::Core::AudioStreamDescriptor;
    using Windows::Media::MediaProperties::MediaEncodingProfile;
    using Windows::Media::MediaProperties::VideoEncodingQuality;
    using Windows::Media::MediaProperties::VideoEncodingProperties;
    using Windows::Media::MediaProperties::AudioEncodingProperties;
    using Windows::Media::MediaProperties::MediaEncodingSubtypes;
    using Windows::Media::Transcoding::MediaTranscoder;
    using Windows::Storage::FileAccessMode;
    using Windows::Storage::Streams::Buffer;
    using Windows::Storage::Streams::InMemoryRandomAccessStream;
}
namespace
{
    UINT64 GetCaptureTimestamp()
    {
        LARGE_INTEGER Counter = {};
        LARGE_INTEGER Frequency = {};
        winrt::check_bool(::QueryPerformanceCounter(&Counter));
        winrt::check_bool(::QueryPerformanceFrequency(&Frequency));
        return Counter.QuadPart / Frequency.QuadPart * 10000000ULL +
            Counter.QuadPart % Frequency.QuadPart * 10000000ULL /
                Frequency.QuadPart;
    }
    bool WaitForCaptureData(
        _In_ HANDLE Data,
        _In_opt_ HANDLE End)
    {
        HANDLE Events[2] = {};
        Events[0] = End ? End : Data;
        Events[1] = Data;
        DWORD Result = ::WaitForMultipleObjects(
            End ? 2 : 1,
            Events,
            FALSE,
            INFINITE);
        winrt::check_bool(Result != WAIT_FAILED);
        return Result == WAIT_OBJECT_0 + (End ? 1 : 0);
    }
    class WindowCapture : public Mile::ComObject<WindowCapture, IUnknown>
    {
    public:
        HWND WindowHandle = nullptr;
        HWND ToolbarHandle = nullptr;
        winrt::IDirect3DDevice Device = nullptr;
        winrt::Direct3D11CaptureFramePool Pool = nullptr;
        winrt::GraphicsCaptureSession Session = nullptr;
        winrt::handle Available;
        winrt::event_token Arrived = {};
        SIZE Canvas = {};
        winrt::Direct3D11CaptureFrame Image = nullptr;
        UINT64 Timestamp = 0;
        HRESULT Status = S_OK;
        bool Closing = false;
        std::mutex Mutex;

        RECT Region()
        {
            RECT Result = {};
            RECT Toolbar = {};
            winrt::check_bool(::GetClientRect(this->WindowHandle, &Result));
            winrt::check_bool(::GetClientRect(this->ToolbarHandle, &Toolbar));
            Result.top = Toolbar.bottom;
            winrt::check_bool(Result.right > 0 && Result.bottom > Result.top);
            return Result;
        }
        void Initialize()
        {
            winrt::com_ptr<ID3D11Device> Native;
            HRESULT Result = ::D3D11CreateDevice(
                nullptr,
                D3D_DRIVER_TYPE_HARDWARE,
                nullptr,
                D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                nullptr,
                0,
                D3D11_SDK_VERSION,
                Native.put(),
                nullptr,
                nullptr);
            if (FAILED(Result))
            {
                Result = ::D3D11CreateDevice(
                    nullptr,
                    D3D_DRIVER_TYPE_WARP,
                    nullptr,
                    D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                    nullptr,
                    0,
                    D3D11_SDK_VERSION,
                    Native.put(),
                    nullptr,
                    nullptr);
            }
            winrt::check_hresult(Result);
            winrt::com_ptr<IInspectable> Interface;
            winrt::check_hresult(::CreateDirect3D11DeviceFromDXGIDevice(
                Native.as<IDXGIDevice>().get(),
                Interface.put()));
            this->Device = Interface.as<winrt::IDirect3DDevice>();
            winrt::GraphicsCaptureItem Item = nullptr;
            winrt::check_hresult(winrt::get_activation_factory<
                winrt::GraphicsCaptureItem,
                IGraphicsCaptureItemInterop>()->CreateForWindow(
                    this->WindowHandle,
                    winrt::guid_of<winrt::GraphicsCaptureItem>(),
                    winrt::put_abi(Item)));
            this->Pool = winrt::Direct3D11CaptureFramePool::CreateFreeThreaded(
                this->Device,
                winrt::DirectXPixelFormat::B8G8R8A8UIntNormalized,
                3,
                Item.Size());
            this->Available.attach(::CreateEventW(
                nullptr,
                FALSE,
                FALSE,
                nullptr));
            winrt::check_bool(this->Available.get());
            winrt::com_ptr<WindowCapture> Instance;
            Instance.copy_from(this);
            this->Arrived = this->Pool.FrameArrived(
                [Instance](
                    auto const& Sender,
                    auto const& Arguments)
            {
                UNREFERENCED_PARAMETER(Arguments);
                try
                {
                    std::lock_guard<std::mutex> Lock(Instance->Mutex);
                    if (Instance->Closing)
                    {
                        return;
                    }
                    Instance->Image = Sender.TryGetNextFrame();
                    ::SetEvent(Instance->Available.get());
                }
                catch (...)
                {
                    std::lock_guard<std::mutex> Lock(Instance->Mutex);
                    if (SUCCEEDED(Instance->Status))
                    {
                        Instance->Status = winrt::to_hresult();
                    }
                    ::SetEvent(Instance->Available.get());
                }
            });
            this->Session = this->Pool.CreateCaptureSession(Item);
            this->Session.IsCursorCaptureEnabled(false);
            this->Session.StartCapture();
        }
        winrt::Buffer Read()
        {
            winrt::Direct3D11CaptureFrame Frame = nullptr;
            {
                std::lock_guard<std::mutex> Lock(this->Mutex);
                winrt::check_hresult(this->Status);
                Frame = std::move(this->Image);
            }
            if (!Frame)
            {
                return nullptr;
            }
            auto Size = Frame.ContentSize();
            auto Description = Frame.Surface().Description();
            if (Size.Width != Description.Width ||
                Size.Height != Description.Height)
            {
                Frame.Close();
                Frame = nullptr;
                if (Size.Width > 0 && Size.Height > 0)
                {
                    this->Pool.Recreate(
                        this->Device,
                        winrt::DirectXPixelFormat::B8G8R8A8UIntNormalized,
                        3,
                        Size);
                }
                return nullptr;
            }
            auto Bitmap = winrt::SoftwareBitmap::CreateCopyFromSurfaceAsync(
                Frame.Surface()).get();
            this->Timestamp = Frame.SystemRelativeTime().count();
            Frame.Close();
            Frame = nullptr;
            RECT Area = this->Region();
            RECT Bounds = {};
            POINT Origin = {};
            winrt::check_bool(::ClientToScreen(this->WindowHandle, &Origin));
            winrt::check_hresult(::DwmGetWindowAttribute(
                this->WindowHandle,
                DWMWA_EXTENDED_FRAME_BOUNDS,
                &Bounds,
                sizeof(Bounds)));
            LONG X = Origin.x - Bounds.left;
            LONG Y = Origin.y - Bounds.top + Area.top;
            LONG Width = Area.right;
            LONG Height = Area.bottom - Area.top;
            if (X < 0 || Y < 0 || X + Width > Bitmap.PixelWidth() ||
                Y + Height > Bitmap.PixelHeight())
            {
                return nullptr;
            }
            LONG OutputWidth = this->Canvas.cx ? this->Canvas.cx : Width;
            LONG OutputHeight = this->Canvas.cy ? this->Canvas.cy : Height;
            winrt::Buffer Pixels(OutputWidth * OutputHeight * 4);
            Pixels.Length(Pixels.Capacity());
            std::memset(Pixels.data(), 0, Pixels.Length());
            winrt::Buffer Source(
                Bitmap.PixelWidth() * Bitmap.PixelHeight() * 4);
            Source.Length(Source.Capacity());
            Bitmap.CopyToBuffer(Source);
            Width = (std::min)(Width, OutputWidth);
            Height = (std::min)(Height, OutputHeight);
            for (LONG Row = 0; Row < Height; ++Row)
            {
                std::memcpy(
                    Pixels.data() + Row * OutputWidth * 4,
                    Source.data() + ((Y + Row) * Bitmap.PixelWidth() + X) * 4,
                    Width * 4);
            }
            this->Canvas.cx = OutputWidth;
            this->Canvas.cy = OutputHeight;
            return Pixels;
        }
        void Close()
        {
            {
                std::lock_guard<std::mutex> Lock(this->Mutex);
                this->Closing = true;
                this->Image = nullptr;
            }
            if (this->Session)
            {
                this->Session.Close();
            }
            if (this->Pool)
            {
                this->Pool.FrameArrived(this->Arrived);
                this->Pool.Close();
            }
        }
    };
    class AudioCapture : public Mile::ComObject<
        AudioCapture,
        IActivateAudioInterfaceCompletionHandler,
        IAgileObject>
    {
    public:
        winrt::com_ptr<IAudioClient> Client;
        winrt::com_ptr<IAudioCaptureClient> Reader;
        winrt::handle Available;
        HRESULT Result = E_PENDING;
        UINT64 NextTime = 0;
        bool Ending = false;
        winrt::handle SamplesAvailable;
        winrt::handle Thread;
        std::mutex Mutex;
        std::vector<winrt::MediaStreamSample> Samples;
        std::vector<winrt::MediaStreamSample> PendingSamples;
        std::size_t NextSample = 0;
        HRESULT Failure = S_OK;
        bool Finished = false;

        HRESULT STDMETHODCALLTYPE ActivateCompleted(
            _In_ IActivateAudioInterfaceAsyncOperation* Operation) override
        {
            winrt::com_ptr<IUnknown> Interface;
            HRESULT Status = Operation->GetActivateResult(
                &this->Result,
                Interface.put());
            if (FAILED(Status))
            {
                this->Result = Status;
            }
            if (SUCCEEDED(this->Result))
            {
                this->Result = Interface->QueryInterface(
                    __uuidof(IAudioClient),
                    this->Client.put_void());
            }
            ::SetEvent(this->Available.get());
            return S_OK;
        }
        void Initialize()
        {
            this->Available.attach(::CreateEventW(
                nullptr,
                FALSE,
                FALSE,
                nullptr));
            winrt::check_bool(this->Available.get());
            AUDIOCLIENT_ACTIVATION_PARAMS Parameters = {};
            Parameters.ActivationType =
                AUDIOCLIENT_ACTIVATION_TYPE_PROCESS_LOOPBACK;
            Parameters.ProcessLoopbackParams.TargetProcessId =
                ::GetCurrentProcessId();
            Parameters.ProcessLoopbackParams.ProcessLoopbackMode =
                PROCESS_LOOPBACK_MODE_INCLUDE_TARGET_PROCESS_TREE;
            PROPVARIANT Arguments = {};
            Arguments.vt = VT_BLOB;
            Arguments.blob.cbSize = sizeof(Parameters);
            Arguments.blob.pBlobData = reinterpret_cast<BYTE*>(&Parameters);
            winrt::com_ptr<IActivateAudioInterfaceAsyncOperation> Operation;
            winrt::check_hresult(::ActivateAudioInterfaceAsync(
                VIRTUAL_AUDIO_DEVICE_PROCESS_LOOPBACK,
                __uuidof(IAudioClient),
                &Arguments,
                this,
                Operation.put()));
            ::WaitForCaptureData(this->Available.get(), nullptr);
            winrt::check_hresult(this->Result);
            WAVEFORMATEX Format = {};
            Format.wFormatTag = WAVE_FORMAT_PCM;
            Format.nChannels = 2;
            Format.nSamplesPerSec = 48000;
            Format.wBitsPerSample = 16;
            Format.nBlockAlign = 4;
            Format.nAvgBytesPerSec = 192000;
            winrt::check_hresult(this->Client->Initialize(
                AUDCLNT_SHAREMODE_SHARED,
                AUDCLNT_STREAMFLAGS_LOOPBACK |
                    AUDCLNT_STREAMFLAGS_EVENTCALLBACK |
                    AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                10000000,
                0,
                &Format,
                nullptr));
            winrt::check_hresult(this->Client->GetService(
                __uuidof(IAudioCaptureClient),
                this->Reader.put_void()));
            winrt::check_hresult(this->Client->SetEventHandle(
                this->Available.get()));
            winrt::check_hresult(this->Client->Start());
        }
        winrt::MediaStreamSample ReadPacket(
            _In_ HANDLE StopEvent,
            _In_ UINT64 Epoch)
        {
            for (;;)
            {
                if (!this->Ending && ::WaitForMultipleObjects(
                    1,
                    &StopEvent,
                    FALSE,
                    0) == WAIT_OBJECT_0)
                {
                    winrt::check_hresult(this->Client->Stop());
                    this->Ending = true;
                }
                BYTE* Data = nullptr;
                UINT32 Frames = 0;
                DWORD Flags = 0;
                UINT64 Time = 0;
                HRESULT Status = this->Reader->GetBuffer(
                    &Data,
                    &Frames,
                    &Flags,
                    nullptr,
                    &Time);
                winrt::check_hresult(Status);
                if (Status == AUDCLNT_S_BUFFER_EMPTY)
                {
                    if (this->Ending)
                    {
                        return nullptr;
                    }
                    ::WaitForCaptureData(this->Available.get(), StopEvent);
                    continue;
                }
                winrt::Buffer Bytes = nullptr;
                {
                    auto Release = Mile::ScopeExitTaskHandler([&]()
                    {
                        this->Reader->ReleaseBuffer(Frames);
                    });
                    Bytes = winrt::Buffer(Frames * 4);
                    Bytes.Length(Frames * 4);
                    if (Flags & AUDCLNT_BUFFERFLAGS_SILENT)
                    {
                        std::memset(Bytes.data(), 0, Bytes.Length());
                    }
                    else
                    {
                        std::memcpy(Bytes.data(), Data, Bytes.Length());
                    }
                }
                if (!(Flags & AUDCLNT_BUFFERFLAGS_TIMESTAMP_ERROR))
                {
                    this->NextTime = Time > Epoch ? Time - Epoch : 0;
                }
                auto Sample = winrt::MediaStreamSample::CreateFromBuffer(
                    Bytes,
                    winrt::TimeSpan(this->NextTime));
                UINT64 Duration = Frames * 10000000ULL / 48000;
                Sample.Duration(winrt::TimeSpan(Duration));
                Sample.Discontinuous(!!(Flags &
                    AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY));
                this->NextTime += Duration;
                return Sample;
            }
        }
        void Start(
            _In_ HANDLE StopEvent,
            _In_ UINT64 Epoch)
        {
            this->SamplesAvailable.attach(::CreateEventW(
                nullptr,
                FALSE,
                FALSE,
                nullptr));
            winrt::check_bool(this->SamplesAvailable.get());
            winrt::com_ptr<AudioCapture> Instance;
            Instance.copy_from(this);
            this->Thread.attach(Mile::CreateThread(
                [Instance, StopEvent, Epoch]()
            {
                ::SetThreadDescription(
                    ::GetCurrentThread(),
                    L"NanaBox.Capture.Thread.Audio");
                try
                {
                    winrt::init_apartment();
                    auto Uninitialize = Mile::ScopeExitTaskHandler([]()
                    {
                        winrt::uninit_apartment();
                    });
                    while (auto Sample = Instance->ReadPacket(StopEvent, Epoch))
                    {
                        std::lock_guard<std::mutex> Lock(Instance->Mutex);
                        Instance->Samples.push_back(std::move(Sample));
                        ::SetEvent(Instance->SamplesAvailable.get());
                    }
                }
                catch (...)
                {
                    std::lock_guard<std::mutex> Lock(Instance->Mutex);
                    Instance->Failure = winrt::to_hresult();
                    ::SetEvent(StopEvent);
                }
                {
                    std::lock_guard<std::mutex> Lock(Instance->Mutex);
                    Instance->Finished = true;
                }
                ::SetEvent(Instance->SamplesAvailable.get());
            }));
            winrt::check_bool(this->Thread.get());
        }
        winrt::MediaStreamSample Read()
        {
            if (this->NextSample == this->PendingSamples.size())
            {
                this->PendingSamples.clear();
                this->NextSample = 0;
                for (;;)
                {
                    {
                        std::lock_guard<std::mutex> Lock(this->Mutex);
                        winrt::check_hresult(this->Failure);
                        if (!this->Samples.empty())
                        {
                            this->PendingSamples.swap(this->Samples);
                            break;
                        }
                        if (this->Finished)
                        {
                            return nullptr;
                        }
                    }
                    ::WaitForCaptureData(this->SamplesAvailable.get(), nullptr);
                }
            }
            return std::move(this->PendingSamples[this->NextSample++]);
        }
    };
}
winrt::IRandomAccessStream NanaBox::CaptureVirtualMachine(
    _In_ HWND WindowHandle,
    _In_ HWND ToolbarHandle,
    _In_opt_ HANDLE StopEvent,
    std::wstring const& FilePath,
    std::function<void()> const& RecordingStarted)
{
    winrt::com_ptr<WindowCapture> Capture;
    Capture.attach(new WindowCapture());
    Capture->WindowHandle = WindowHandle;
    Capture->ToolbarHandle = ToolbarHandle;
    auto Close = Mile::ScopeExitTaskHandler([&]()
    {
        Capture->Close();
    });
    Capture->Initialize();
    if (FilePath.empty())
    {
        winrt::Buffer Pixels = nullptr;
        do
        {
            if (!::WaitForCaptureData(Capture->Available.get(), StopEvent))
            {
                return nullptr;
            }
            Pixels = Capture->Read();
        } while (!Pixels);
        SIZE Canvas = Capture->Canvas;
        winrt::InMemoryRandomAccessStream Stream;
        auto Encoder = winrt::BitmapEncoder::CreateAsync(
            winrt::BitmapEncoder::PngEncoderId(),
            Stream).get();
        Encoder.SetSoftwareBitmap(winrt::SoftwareBitmap::CreateCopyFromBuffer(
            Pixels,
            winrt::BitmapPixelFormat::Bgra8,
            Canvas.cx,
            Canvas.cy));
        Encoder.FlushAsync().get();
        return Stream;
    }
    RECT Area = Capture->Region();
    SIZE Canvas = {};
    Canvas.cx = (Area.right + 1) & ~1;
    Canvas.cy = (Area.bottom - Area.top + 1) & ~1;
    Capture->Canvas = Canvas;
    winrt::com_ptr<AudioCapture> Sound;
    UINT64 Epoch = ::GetCaptureTimestamp();
    auto Stop = Mile::ScopeExitTaskHandler([&]()
    {
        ::SetEvent(StopEvent);
        if (Sound && Sound->Thread)
        {
            ::WaitForCaptureData(Sound->Thread.get(), nullptr);
        }
    });
    if (::MileIsWindowsVersionAtLeast(10, 0, 20348))
    {
        Sound.attach(new AudioCapture());
        Sound->Initialize();
        Sound->Start(StopEvent, Epoch);
    }
    auto Input = winrt::VideoEncodingProperties::CreateUncompressed(
        winrt::MediaEncodingSubtypes::Bgra8(),
        Canvas.cx,
        Canvas.cy);
    Input.FrameRate().Numerator(60);
    Input.FrameRate().Denominator(1);
    Input.Properties().Insert(
        MF_MT_DEFAULT_STRIDE,
        winrt::box_value(UINT32(Canvas.cx * 4)));
    winrt::VideoStreamDescriptor Video(Input);
    winrt::AudioStreamDescriptor Audio = nullptr;
    if (Sound)
    {
        Audio = winrt::AudioStreamDescriptor(
            winrt::AudioEncodingProperties::CreatePcm(48000, 2, 16));
    }
    auto Profile = winrt::MediaEncodingProfile::CreateMp4(
        winrt::VideoEncodingQuality::Auto);
    Profile.Video().Width(Canvas.cx);
    Profile.Video().Height(Canvas.cy);
    Profile.Video().Properties().Insert(
        MF_MT_INTERLACE_MODE,
        winrt::box_value(UINT32(MFVideoInterlace_Progressive)));
    if (!Audio)
    {
        Profile.Audio(nullptr);
    }
    winrt::MediaStreamSource Source = Audio ?
        winrt::MediaStreamSource(Video, Audio) :
        winrt::MediaStreamSource(Video);
    Source.IsLive(true);
    Source.BufferTime(winrt::TimeSpan::zero());
    auto Requested = Source.SampleRequested(
        winrt::auto_revoke,
        [Capture, Video, Sound, StopEvent, Epoch,
            LastImage = winrt::Buffer(nullptr), VideoEnded = false](
                auto const& Sender,
                auto const& Arguments) mutable
    {
        try
        {
            bool Picture = Arguments.Request().StreamDescriptor() == Video;
            winrt::MediaStreamSample Sample = nullptr;
            if (Picture && !VideoEnded)
            {
                bool Ready = ::WaitForCaptureData(
                    Capture->Available.get(), StopEvent);
                winrt::Buffer Pixels = nullptr;
                while (Ready && !(Pixels = Capture->Read()))
                {
                    Ready = ::WaitForCaptureData(
                        Capture->Available.get(), StopEvent);
                }
                if (!Ready)
                {
                    Pixels = LastImage;
                }
                UINT64 Time = Ready && Capture->Timestamp > Epoch ?
                    Capture->Timestamp - Epoch :
                    ::GetCaptureTimestamp() - Epoch;
                VideoEnded = !Ready;
                if (Pixels)
                {
                    LastImage = Pixels;
                    Sample = winrt::MediaStreamSample::CreateFromBuffer(
                        Pixels,
                        winrt::TimeSpan(Time));
                    Sample.Duration(winrt::TimeSpan(10000000 / 60));
                }
            }
            else if (!Picture)
            {
                Sample = Sound->Read();
            }
            Arguments.Request().Sample(Sample);
        }
        catch (...)
        {
            {
                std::lock_guard<std::mutex> Lock(Capture->Mutex);
                if (SUCCEEDED(Capture->Status))
                {
                    Capture->Status = winrt::to_hresult();
                }
            }
            ::SetEvent(StopEvent);
            Sender.NotifyError(winrt::Windows::Media::Core::
                MediaStreamSourceErrorStatus::Other);
        }
    });
    {
        winrt::file_handle File(::CreateFileW(
            FilePath.c_str(),
            GENERIC_WRITE,
            0,
            nullptr,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr));
        winrt::check_bool(File.get() != INVALID_HANDLE_VALUE);
    }
    winrt::IRandomAccessStream Output = nullptr;
    winrt::check_hresult(::CreateRandomAccessStreamOnFile(
        FilePath.c_str(),
        DWORD(winrt::FileAccessMode::ReadWrite),
        winrt::guid_of<winrt::IRandomAccessStream>(),
        winrt::put_abi(Output)));
    winrt::MediaTranscoder Encoder;
    auto Prepared = Encoder.PrepareMediaStreamSourceTranscodeAsync(
        Source,
        Output,
        Profile).get();
    winrt::check_bool(Prepared.CanTranscode());
    RecordingStarted();
    try
    {
        Prepared.TranscodeAsync().get();
    }
    catch (...)
    {
        std::lock_guard<std::mutex> Lock(Capture->Mutex);
        winrt::check_hresult(Capture->Status);
        throw;
    }
    {
        std::lock_guard<std::mutex> Lock(Capture->Mutex);
        winrt::check_hresult(Capture->Status);
    }
    winrt::check_bool(Output.FlushAsync().get());
    Output.Close();
    return nullptr;
}
