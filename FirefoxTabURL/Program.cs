using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Windows.Automation;
using System.Windows.Forms;
using System.Threading;

namespace List_all_opened_tabs_of_Firefox
{
    class Program
    {
        static void Main(string[] args)
        {
            List<string> firefoxUrls = new List<string>();

            Process[] processes = Process.GetProcessesByName("firefox");
            IntPtr wantedHandle = new IntPtr();
            for (int i = 0; i < processes.Length; i++)
                if (!IntPtr.Equals(processes[i].MainWindowHandle, IntPtr.Zero))
                {
                    wantedHandle = processes[i].MainWindowHandle;
                    break;
                }

            //Process firefox = Process.GetProcessesByName("firefox")[0];
            AutomationElement rootElement = AutomationElement.FromHandle(wantedHandle);            
            

            //Console.WriteLine(DateTime.Now);
            //Condition condDocAll = new PropertyCondition(AutomationElement.ControlTypeProperty, ControlType.Document);
            //foreach (AutomationElement docElement in rootElement.FindAll(TreeScope.Descendants, condDocAll))
            //{

            //    foreach (AutomationPattern pattern in docElement.GetSupportedPatterns())
            //    {
            //        if (docElement.GetCurrentPattern(pattern) is ValuePattern)
            //            Console.WriteLine((docElement.GetCurrentPattern(pattern) as ValuePattern).Current.Value.ToString() + Environment.NewLine);
            //    }
            //}
            //Console.WriteLine(DateTime.Now);
            IntPtr ipYouTube = new IntPtr();
            Condition condCustomControl = new PropertyCondition(AutomationElement.ControlTypeProperty, ControlType.Custom);
            AutomationElement firstCustomControl = GetNextCustomControl(rootElement, condCustomControl);

            Condition condToolbarControl = new AndCondition(
                new PropertyCondition(AutomationElement.ControlTypeProperty, ControlType.ToolBar),
                new PropertyCondition(AutomationElement.NameProperty, "Browser-Tabs"));

            AutomationElement toolbarFF = firstCustomControl.FindFirst(TreeScope.Children, condToolbarControl);

            Condition condTabItemControl =new AndCondition(
                new PropertyCondition(AutomationElement.ControlTypeProperty, ControlType.TabItem),
                new PropertyCondition(AutomationElement.NameProperty, "YouTube - Broadcast Yourself"));

            AutomationElement youTubeTab = toolbarFF.FindFirst(TreeScope.Descendants, condTabItemControl);
            (youTubeTab.GetCurrentPattern(InvokePattern.Pattern) as InvokePattern).Invoke();


            AutomationElement secondCustomControl = GetNextCustomControl(firstCustomControl, condCustomControl);
            foreach (AutomationElement thirdElement in secondCustomControl.FindAll(TreeScope.Children, condCustomControl))
            {
                foreach (AutomationElement fourthElement in thirdElement.FindAll(TreeScope.Children, condCustomControl))
                {
                    Condition condDocument = new PropertyCondition(AutomationElement.ControlTypeProperty, ControlType.Document);
                    AutomationElement docElement = fourthElement.FindFirst(TreeScope.Children, condDocument);
                    if (docElement != null)
                    {
                        foreach (AutomationPattern pattern in docElement.GetSupportedPatterns())
                        {
                            if (docElement.GetCurrentPattern(pattern) is ValuePattern)
                            {
                                Console.WriteLine((docElement.GetCurrentPattern(pattern) as ValuePattern).Current.Value.ToString() + Environment.NewLine);
                                if ((docElement.GetCurrentPattern(pattern) as ValuePattern).Current.Value.ToString().Contains("youtube.com"))
                                {
                                    ipYouTube = new IntPtr(fourthElement.Current.NativeWindowHandle);
                                    AutomationElement flashElement = docElement.FindAll(TreeScope.Children, condCustomControl)[1].
                                        FindAll(TreeScope.Children, condCustomControl)[2]
                                        .FindFirst(TreeScope.Children, condCustomControl)
                                        .FindFirst(TreeScope.Children, condCustomControl)
                                        .FindAll(TreeScope.Children, condCustomControl)[1]
                                        .FindFirst(TreeScope.Children, condCustomControl)
                                        .FindAll(TreeScope.Children, condCustomControl)[0];
                                    //flashElement.SetFocus();
                                    (flashElement.GetCurrentPattern(InvokePattern.Pattern) as InvokePattern).Invoke();
                                    Keys key = Keys.D0;
                                    //SendKeys.SendWait(key.ToString());
                                    //SendKeys.SendWait("SPACE");
                                    SendKeys.SendWait(key.ToString());
                                }
                            }
                        }
                    }
                }
            }

            Keys keySpace = Keys.Up;

            WinAPI.PostMessage(ipYouTube, WinAPI.WM_KEYDOWN, (uint)keySpace, 0);
            Thread.Sleep(100);
            WinAPI.PostMessage(ipYouTube, WinAPI.WM_KEYUP, (uint)keySpace, 0);


            Console.WriteLine(DateTime.Now);
            Console.ReadLine();
        }
        private static AutomationElement GetNextCustomControl(AutomationElement rootElement, Condition condCustomControl)
        {
            return rootElement.FindAll(TreeScope.Children, condCustomControl).Cast<AutomationElement>().ToList().Where(x => x.Current.BoundingRectangle != System.Windows.Rect.Empty).FirstOrDefault();                      
        }
    }
}
