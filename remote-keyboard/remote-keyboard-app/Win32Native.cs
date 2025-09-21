/*
*	remote-keyboard-app - Win32Native.cs
*	원격 가상 키보드 장치를 구현하는 예제입니다.
*
*	Copyright (C) 2025-2026 Coppermine-SP
*/

using System.Runtime.InteropServices;
using System.Net.Sockets;

namespace remote_keyboard_app
{
    internal static class Win32Native
    {

        public static IntPtr s_hook = IntPtr.Zero;
        public static LowLevelKeyboardProc? s_proc; // delegate GC 보호용

        public const int WH_KEYBOARD_LL = 13;
        public const int WM_KEYDOWN = 0x0100;
        public const int WM_KEYUP = 0x0101;
        public const int WM_SYSKEYDOWN = 0x0104;
        public const int WM_SYSKEYUP = 0x0105;

        public const int VK_LCONTROL = 0xA2, VK_RCONTROL = 0xA3;
        public const int VK_LSHIFT = 0xA0, VK_RSHIFT = 0xA1;
        public const int VK_LMENU = 0xA4, VK_RMENU = 0xA5;  
        public const int VK_LWIN = 0x5B, VK_RWIN = 0x5C;    

        [StructLayout(LayoutKind.Sequential)]
        public struct KBDLLHOOKSTRUCT
        {
            public uint vkCode;
            public uint scanCode;
            public uint flags;
            public uint time;
            public IntPtr dwExtraInfo;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct MSG
        {
            public IntPtr hwnd;
            public uint message;
            public UIntPtr wParam;
            public IntPtr lParam;
            public uint time;
            public int pt_x;
            public int pt_y;
        }

        public delegate IntPtr LowLevelKeyboardProc(int nCode, IntPtr wParam, IntPtr lParam);

        [DllImport("user32.dll", SetLastError = true)]
        public static extern IntPtr SetWindowsHookEx(int idHook, LowLevelKeyboardProc lpfn, IntPtr hMod, uint dwThreadId);

        [DllImport("user32.dll", SetLastError = true)]
        public static extern bool UnhookWindowsHookEx(IntPtr hhk);

        [DllImport("user32.dll", SetLastError = true)]
        public static extern IntPtr CallNextHookEx(IntPtr hhk, int nCode, IntPtr wParam, IntPtr lParam);

        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        public static extern IntPtr GetModuleHandle(string? lpModuleName);

        [DllImport("user32.dll")]
        public static extern sbyte GetMessage(out MSG lpMsg, IntPtr hWnd, uint wMsgFilterMin, uint wMsgFilterMax);

        [DllImport("user32.dll")]
        public static extern bool TranslateMessage([In] ref MSG lpMsg);

        [DllImport("user32.dll")]
        public static extern IntPtr DispatchMessage([In] ref MSG lpMsg);

        public static readonly Dictionary<int, byte> VkToUsage = BuildVkToUsageMap();

        public static Dictionary<int, byte> BuildVkToUsageMap()
        {
            var m = new Dictionary<int, byte>(256);

            // 알파벳
            for (int vk = 0x41, usage = 0x04; vk <= 0x5A; vk++, usage++) // 'A'..'Z' -> 0x04..0x1D
                m[vk] = (byte)usage;

            // 숫자키(상단 숫자열)
            m[0x31] = 0x1E; // '1'
            m[0x32] = 0x1F; // '2'
            m[0x33] = 0x20; // '3'
            m[0x34] = 0x21; // '4'
            m[0x35] = 0x22; // '5'
            m[0x36] = 0x23; // '6'
            m[0x37] = 0x24; // '7'
            m[0x38] = 0x25; // '8'
            m[0x39] = 0x26; // '9'
            m[0x30] = 0x27; // '0'

            // 제어/편집/공백
            m[0x0D] = 0x28; // Enter
            m[0x1B] = 0x29; // Escape
            m[0x08] = 0x2A; // Backspace
            m[0x09] = 0x2B; // Tab
            m[0x20] = 0x2C; // Space

            // 기호(미국식 키보드 기준 VK_OEM_x)
            m[0xBD] = 0x2D; // OEM_MINUS (- _)
            m[0xBB] = 0x2E; // OEM_PLUS (= +)
            m[0xDB] = 0x2F; // OEM_4 ([ {)
            m[0xDD] = 0x30; // OEM_6 (] })
            m[0xDC] = 0x31; // OEM_5 (\ |)
            m[0xBA] = 0x33; // OEM_1 (; :)
            m[0xDE] = 0x34; // OEM_7 (' ")
            m[0xC0] = 0x35; // OEM_3 (` ~)
            m[0xBC] = 0x36; // OEM_COMMA (, <)
            m[0xBE] = 0x37; // OEM_PERIOD (. >)
            m[0xBF] = 0x38; // OEM_2 (/ ?)
            // m[0xE2] = 0x64; // OEM_102(국제키, 필요 시)

            // 기능키
            for (int vk = 0x70, usage = 0x3A; vk <= 0x7B; vk++, usage++) // F1..F12
                m[vk] = (byte)usage;

            // 탐색/편집
            m[0x2C] = 0x46; // PrintScreen(VK_SNAPSHOT)
            m[0x91] = 0x47; // ScrollLock
            m[0x13] = 0x48; // Pause
            m[0x2D] = 0x49; // Insert
            m[0x24] = 0x4A; // Home
            m[0x21] = 0x4B; // PageUp (VK_PRIOR)
            m[0x2E] = 0x4C; // Delete
            m[0x23] = 0x4D; // End
            m[0x22] = 0x4E; // PageDown (VK_NEXT)
            m[0x27] = 0x4F; // Right
            m[0x25] = 0x50; // Left
            m[0x28] = 0x51; // Down
            m[0x26] = 0x52; // Up
            m[0x14] = 0x39; // CapsLock
            m[0x90] = 0x53; // NumLock

            // 키패드(필요 시 확장)
            m[0x6F] = 0x54; // Numpad /
            m[0x6A] = 0x55; // Numpad *
            m[0x6D] = 0x56; // Numpad -
            m[0x6B] = 0x57; // Numpad +
            m[0x6E] = 0x63; // Numpad .
            m[0x61] = 0x59; // Numpad 1
            m[0x62] = 0x5A;
            m[0x63] = 0x5B;
            m[0x64] = 0x5C;
            m[0x65] = 0x5D;
            m[0x66] = 0x5E;
            m[0x67] = 0x5F;
            m[0x68] = 0x60;
            m[0x69] = 0x61;
            m[0x60] = 0x62; // Numpad 0

            // Application(Menu) 키
            m[0x5D] = 0x65; // VK_APPS -> Application
            return m;
        }
    }
}
