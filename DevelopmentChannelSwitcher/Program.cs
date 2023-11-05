using Mile.Project.Helpers;
using System.Text;

namespace DevelopmentChannelSwitcher
{
    internal class Program
    {
        static void Main(string[] args)
        {
            string ProjectRootPath = GitRepository.GetRootPath();
            Console.WriteLine(ProjectRootPath);

            bool Preview = true;

            {
                List<(string Release, string Preview)> ReplacementStringList =
                    new List<(string Release, string Preview)>();
                ReplacementStringList.Add((
                    "DisplayName=\"NanaBox\"",
                    "DisplayName=\"NanaBox Preview\""));
                ReplacementStringList.Add((
                    "Name=\"40174MouriNaruto.NanaBox\"",
                    "Name=\"40174MouriNaruto.NanaBoxPreview\""));
                ReplacementStringList.Add((
                    "<DisplayName>NanaBox</DisplayName>",
                    "<DisplayName>NanaBox Preview</DisplayName>"));
                ReplacementStringList.Add((
                    "AC0B3611-471C-4D75-B175-232BE1174B29",
                    "9466E986-013C-420E-876B-42E4744E9893"));

                string FilePath = string.Format(
                    @"{0}\NanaBoxPackage\Package.appxmanifest",
                    ProjectRootPath);

                string Content = File.ReadAllText(
                    FilePath,
                    Encoding.UTF8);
                foreach (var ReplacementStringItem in ReplacementStringList)
                {
                    if (Preview)
                    {
                        Content = Content.Replace(
                            ReplacementStringItem.Release,
                            ReplacementStringItem.Preview);
                    }
                    else
                    {
                        Content = Content.Replace(
                            ReplacementStringItem.Preview,
                            ReplacementStringItem.Release);
                    }
                }

                FileUtilities.SaveTextToFileAsUtf8Bom(FilePath, Content);
            }

            {
                List<(string Release, string Preview)> ReplacementStringList =
                    new List<(string Release, string Preview)>();
                ReplacementStringList.Add((
                    "return ::SHStrDupW(L\"NanaBox\", ppszName);",
                    "return ::SHStrDupW(L\"NanaBox Preview\", ppszName);"));
                ReplacementStringList.Add((
                    "AC0B3611-471C-4D75-B175-232BE1174B29",
                    "9466E986-013C-420E-876B-42E4744E9893"));

                string FilePath = string.Format(
                    @"{0}\NanaBoxShellExtension\NanaBoxShellExtension.cpp",
                    ProjectRootPath);

                string Content = File.ReadAllText(
                    FilePath,
                    Encoding.UTF8);
                foreach (var ReplacementStringItem in ReplacementStringList)
                {
                    if (Preview)
                    {
                        Content = Content.Replace(
                            ReplacementStringItem.Release,
                            ReplacementStringItem.Preview);
                    }
                    else
                    {
                        Content = Content.Replace(
                            ReplacementStringItem.Preview,
                            ReplacementStringItem.Release);
                    }
                }

                FileUtilities.SaveTextToFileAsUtf8Bom(FilePath, Content);
            }

            {
                List<(string Release, string Preview)> ReplacementStringList =
                new List<(string Release, string Preview)>();
                ReplacementStringList.Add((
                    "<Content Include=\"..\\Assets\\PackageAssets\\**\\*\">",
                    "<Content Include=\"..\\Assets\\PreviewPackageAssets\\**\\*\">"));

                string FilePath = string.Format(
                    @"{0}\NanaBoxPackage\NanaBoxPackage.wapproj",
                    ProjectRootPath);

                string Content = File.ReadAllText(
                    FilePath,
                    Encoding.UTF8);
                foreach (var ReplacementStringItem in ReplacementStringList)
                {
                    if (Preview)
                    {
                        Content = Content.Replace(
                            ReplacementStringItem.Release,
                            ReplacementStringItem.Preview);
                    }
                    else
                    {
                        Content = Content.Replace(
                            ReplacementStringItem.Preview,
                            ReplacementStringItem.Release);
                    }
                }

                FileUtilities.SaveTextToFileAsUtf8Bom(FilePath, Content);
            }

            {
                List<(string Release, string Preview)> ReplacementStringList =
                new List<(string Release, string Preview)>();
                ReplacementStringList.Add((
                    "NanaBox.ico",
                    "NanaBoxPreview.ico"));

                string FilePath = string.Format(
                    @"{0}\NanaBox\NanaBoxResources.rc",
                    ProjectRootPath);

                string Content = File.ReadAllText(
                    FilePath,
                    Encoding.Unicode);
                foreach (var ReplacementStringItem in ReplacementStringList)
                {
                    if (Preview)
                    {
                        Content = Content.Replace(
                            ReplacementStringItem.Release,
                            ReplacementStringItem.Preview);
                    }
                    else
                    {
                        Content = Content.Replace(
                            ReplacementStringItem.Preview,
                            ReplacementStringItem.Release);
                    }
                }

                File.WriteAllText(
                    FilePath,
                    Content,
                    Encoding.Unicode);
            }

            Console.WriteLine("Hello, World!");
            Console.ReadKey();
        }
    }
}
