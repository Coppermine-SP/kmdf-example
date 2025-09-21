using System.Net.Sockets;
using System.Text;

namespace remote_keyboard_app
{
    internal class Program
    {
        static string ip;
        static int port;
        static void Main(string[] args)
        {
            Console.WriteLine("Welcome to remote_keyboard_app.\nCopyright (C) 2025-2026 Coppermine-SP.\n\n\n");

            Console.Write("Target IPv4 > ");
            ip = Console.ReadLine()!;

            Console.Write("Port > ");
            port = int.Parse(Console.ReadLine()!);

            using(UdpClient client = new UdpClient())
            {
                client.Connect(ip, port);
                Console.WriteLine("Connected to " + ip + ":" + port + "\n");
                client.Send(UTF8Encoding.UTF8.GetBytes("remote_keyboard_app"), UTF8Encoding.UTF8.GetByteCount("remote_keyboard_app"));
            }

        }
    }
}
