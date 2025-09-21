/*
*	remote-keyboard-app - Program.cs
*	원격 가상 키보드 장치를 구현하는 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/
using System.Net.Sockets;
using System.Runtime.InteropServices;
using static remote_keyboard_app.Win32Native;

namespace remote_keyboard_app
{
    internal static class Program
    {
        static string ip;
        static int port;
        static byte s_modifiers = 0;
        static readonly HashSet<byte> s_pressedUsages = new HashSet<byte>();
        static byte[] s_lastReport = new byte[8];
        static UdpClient client = new UdpClient();

        static void Main(string[] args)
        {
            Console.WriteLine("Welcome to remote_keyboard_app.\nCopyright (C) 2025-2026 Coppermine-SP.\n\n\n");

            Console.Write("Target IPv4 > ");
            ip = Console.ReadLine()!;

            Console.Write("Port > ");
            port = int.Parse(Console.ReadLine()!);

                try
                {
                    client.Connect(ip, port);
                    Console.WriteLine("Connected to the target device.");

                    Win32Native.s_proc = HookCallback;
                    IntPtr hMod = Win32Native.GetModuleHandle(null);
                    Win32Native.s_hook = Win32Native.SetWindowsHookEx(Win32Native.WH_KEYBOARD_LL, Win32Native.s_proc, hMod, 0);
                    if(Win32Native.s_hook == IntPtr.Zero)
                    {
                        Console.WriteLine("SetWindowsHookEx failed.");
                        return;
                    }

                    Console.CancelKeyPress += (_, e) =>
                    {
                        e.Cancel = true;
                        Cleanup();
                    };

                    Console.WriteLine("Global keyboard hook installed. Press Ctrl+C to exit.");
                    while (true)
                    {
                        if (GetMessage(out MSG msg, IntPtr.Zero, 0, 0) <= 0) break;
                        TranslateMessage(ref msg);
                        DispatchMessage(ref msg);
                    }


                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Error: {ex.Message}");
                    return;
                }

           
        }

        private static void Cleanup()
        {
            try
            {
                if (s_hook != IntPtr.Zero)
                    Win32Native.UnhookWindowsHookEx(s_hook);

                SendZeroReport();
                client.Dispose();
            }
            catch
            {
                //ignored
            }

            Environment.Exit(0);
        }

        private static bool UpdateModifier(int vk, bool isDown, bool isUp)
        {
            byte before = s_modifiers;

            void SetBit(int bit, bool on)
            {
                if (on) s_modifiers = (byte)(s_modifiers | (1 << bit));
                else s_modifiers = (byte)(s_modifiers & ~(1 << bit));
            }

            switch (vk)
            {
                case VK_LCONTROL: SetBit(0, isDown); if (isUp) SetBit(0, false); break;
                case VK_LSHIFT: SetBit(1, isDown); if (isUp) SetBit(1, false); break;
                case VK_LMENU: SetBit(2, isDown); if (isUp) SetBit(2, false); break;
                case VK_LWIN: SetBit(3, isDown); if (isUp) SetBit(3, false); break;

                case VK_RCONTROL: SetBit(4, isDown); if (isUp) SetBit(4, false); break;
                case VK_RSHIFT: SetBit(5, isDown); if (isUp) SetBit(5, false); break;
                case VK_RMENU: SetBit(6, isDown); if (isUp) SetBit(6, false); break;
                case VK_RWIN: SetBit(7, isDown); if (isUp) SetBit(7, false); break;

                default:
                    return false;
            }

            return s_modifiers != before;
        }

        private static void SendZeroReport()
        {
            byte[] report = new byte[8];
            for (int i = 0; i < 8; i++) report[i] = 0x00;
            s_lastReport = report;
            try
            {
                client.Send(report, 8);
            }
            catch
            {
                //ignored
            }
        }

        private static void SendCurrentReport()
        {
            byte[] report = new byte[8];
            report[0] = s_modifiers; // modifiers
            report[1] = 0x00;        // reserved

            // HID 부트 키보드는 동시 6키 제한 → 초과 시 앞 6개만
            int idx = 2;
            foreach (var usage in s_pressedUsages)
            {
                if (idx >= 8) break; // 6개 채움
                report[idx++] = usage;
            }
            for (; idx < 8; idx++) report[idx] = 0x00;

            // 중복 리포트 억제(선택)
            bool same = true;
            for (int i = 0; i < 8; i++) if (report[i] != s_lastReport[i]) { same = false; break; }
            if (same) return;

            s_lastReport = report;
            try
            {
                client.Send(report, 8);
            }
            catch
            {
                //ignored
            }
        }


        private static IntPtr HookCallback(int nCode, IntPtr wParam, IntPtr lParam)
        {
            try
            {
                if(nCode >= 0)
                {
                    int msg = (int)wParam;
                    bool isDown = (msg == Win32Native.WM_KEYDOWN || msg == Win32Native.WM_SYSKEYDOWN);
                    bool isUp = (msg == Win32Native.WM_KEYUP || msg == Win32Native.WM_SYSKEYUP);

                    var kb = Marshal.PtrToStructure<KBDLLHOOKSTRUCT>(lParam);
                    int vk = unchecked((int)kb.vkCode);

                    if(UpdateModifier(vk, isDown, isUp))
                    {
                        SendCurrentReport();
                        return CallNextHookEx(s_hook, nCode, wParam, lParam);
                    }

                    if(VkToUsage.TryGetValue(vk, out byte usage))
                    {
                        if(isDown)
                            s_pressedUsages.Add(usage);
                        else if(isUp)
                            s_pressedUsages.Remove(usage);


                        SendCurrentReport();
                    }
                }
            }
            catch
            {
                // ignored
            }

            return CallNextHookEx(s_hook, nCode, wParam, lParam);
        }
    }
}
