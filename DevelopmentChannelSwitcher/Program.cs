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

            bool Preview = false;

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

            Console.WriteLine("Hello, World!");
            Console.ReadKey();
        }
    }
}
