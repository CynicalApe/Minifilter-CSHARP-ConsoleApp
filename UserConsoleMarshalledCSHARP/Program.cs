using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using System.IO;
using System.Data.Entity;


namespace UserAppMarshalledCSHARP
{
    static class Constants
    {
        /* Message Buffer Size */
        public const int BufferSize = 1024;

        /* Foo Return PARAMS */
        public const byte SUCCESS_FOUND = 0;
        public const byte SUCCESS_NOT_FOUND = 2;
        public const byte WRONG_FILE_FORMAT = 4;
        public const byte PERMISSIONS_FILE_NOT_FOUND = 8;

        /* Regex Pattern */
        public const string RegexPattern = ":[0157]:[^:;<>\\/?#*;]+[;]";

        /* File Names */
        public const string LOG_FILE = "C:\\log.txt";
        public const string RESTRICTIONS_FILE = "C:\\rest.txt";

        /* Permission Definitions */
        public const byte NO_ACCESS = 0;
        public const byte READ_ONLY = 1;
        public const byte WRITE_ONLY = 4;
        public const byte COMPLETE_ACCESS = 7;
    }

    class Program
    {
        /* --- start of necessary & marshalled structures */
        [StructLayout(LayoutKind.Explicit)]
        public unsafe struct _OVERLAPPED
        {
            [FieldOffset(0)]
            IntPtr Internal;
            [FieldOffset(4)]
            IntPtr InternalHigh;
            [FieldOffset(8)]
            OVER_PIECE Q;
            [FieldOffset(8)]
            System.IntPtr Pointer;
            [FieldOffset(16)]
            System.IntPtr hEvent;
        };

        public unsafe struct OVER_PIECE
        {
            System.UInt32 Offset;
            System.UInt32 OffsetHigh;
        };

        [StructLayout(LayoutKind.Sequential)]
        public struct FILTER_MESSAGE_HEADER
        {
            public uint replyLength;
            public ulong messageId;
        }

        [StructLayout(LayoutKind.Sequential)]
        public unsafe struct MESSY_MESSAGE
        {
            public fixed char buffer[Constants.BufferSize];
        };

        [StructLayout(LayoutKind.Sequential)]
        public unsafe struct _PERM_CODE
        {
            public System.UInt16 PermissionLevel;
        };

        [StructLayout(LayoutKind.Sequential)]
        public unsafe struct _REPLY_MESSAGE
        {
            public _PERM_CODE permission;
        };

        [StructLayout(LayoutKind.Sequential)]
        public unsafe struct JOE
        {
            public FILTER_MESSAGE_HEADER header;
            public MESSY_MESSAGE message;
        };

        [StructLayout(LayoutKind.Sequential)]
        public unsafe struct _REPLY_STRUCTURE
        {
            public FILTER_REPLY_HEADER header;
            public _REPLY_MESSAGE reply;
        };

        [StructLayout(LayoutKind.Sequential)]
        public unsafe struct FILTER_REPLY_HEADER
        {
            public System.Int32 Status;
            public System.UInt64 MessageId;
        };

        [StructLayout(LayoutKind.Sequential)]
        public unsafe struct SECURITY_ATTRIBUTES
        {
            System.UInt32 nLength;
            System.Int32* lpSecurityDescriptor;
            System.Int32 bInheritHandle;
        };

        [DllImport("FltLib.dll")]
        unsafe static extern System.Int32 FilterReplyMessage(
            System.IntPtr hPort,
            FILTER_REPLY_HEADER* lpReplyBuffer,
            System.UInt32 dwReplyBufferSize
        );

        [DllImport("FltLib.dll")]
        unsafe static extern System.Int32 FilterGetMessage(
             System.IntPtr hPort,
             FILTER_MESSAGE_HEADER* lpMessageBuffer,
             System.UInt32 dwMessageBufferSize,
             _OVERLAPPED* lpOverlapped
        );

        [DllImport("FltLib.dll")]
        unsafe static extern System.Int32 FilterConnectCommunicationPort
            (
            [MarshalAs(UnmanagedType.LPWStr)] System.String lpPortName,
            System.UInt32 dwOptions,
            System.IntPtr lpContext,
            System.UInt16 dwSizeOfContext,
            SECURITY_ATTRIBUTES* lpSecurityAttributes,
            System.IntPtr* hPort
       );
        /* end of marshall ---*/


        /* Definitions needed for the entity framework */
        public class LogEntry
        {

            public string Id { get; set; }
        }

        public class LogContext : DbContext
        {
            public DbSet<LogEntry> logs { get; set; }
        }

        public class PermissionEntry
        {
            public string Id { get; set; }
        }

        public class PermissionContext : DbContext
        {
            public DbSet<PermissionEntry> perms { get; set; }
        }


        /* ARGS: Input, string that contains the file name
         * RETURNS: NONE
         * Writes input to the database
         */
        public static void logWrite(string input)
        {
            using (var db = new LogContext())
            {
                Console.WriteLine($"Input{input}");
                var entry = new LogEntry { Id = input };
                var req = db.logs.Find(input);
                if (req != null)
                    return;
                db.logs.Add(entry);
                db.SaveChanges();
            }
        }

        /* ARGS: NONE
        * RETURNS: NONE
        * Reads every entry in the log file 
        */
        public static void logReadAll()
        {
            using (var db = new LogContext())
            {
                var query = from b in db.logs orderby b.Id select b;

                foreach (var item in query)
                {
                    Console.WriteLine(item.Id);
                }
            }
        }

        /* ARGS: Input, string that contains the file name
         * RETURNS: NONE
         * Writes input to the database
         */
        public static void permWrite(string input)
        {
            using (var db = new PermissionContext())
            {
                Console.WriteLine($"Input{input}");
                var entry = new PermissionEntry { Id = input };
                var req = db.perms.Find(input);
                if (req != null)
                    return;
                db.perms.Add(entry);
                db.SaveChanges();
            }
        }

        /* ARGS: NONE
         * RETURNS: NONE
         * Reads every entry in the permission file 
         */
        public static void permReadAll()
        {
            using (var db = new PermissionContext())
            {
                var query = from b in db.perms orderby b.Id select b;
                foreach (var item in query)
                {
                    Console.WriteLine(item.Id);
                }
            }
        }

        /*
         * ARGS: Input, contains the string that will be search in the perm file  
         *       Perm, output paramater that is used to set the permission level 
         * RETURNS: A custom set of success - failure codes 
         */
        public unsafe static int CheckPerm(string input, ref _PERM_CODE perm)
        {

            System.IO.StreamReader file = new System.IO.StreamReader(Constants.RESTRICTIONS_FILE);

            if (file == null)
                return Constants.PERMISSIONS_FILE_NOT_FOUND;

            /* Read restrictions line by line */
            using (var db = new PermissionContext())
            {
                var query = from b in db.perms orderby b.Id select b;

                foreach (var item in query)
                {

                    /* if there is a pattern miss match return wrong file format */
                    if (!Regex.IsMatch(item.Id, Constants.RegexPattern))
                    {
                        perm.PermissionLevel = Constants.COMPLETE_ACCESS;
                        return Constants.WRONG_FILE_FORMAT;
                    }

                    /* if the requested file is in the restrictions list, 
                     * get its permission level and set it to the output paramater 
                     so it can be sent to the driver */
                    if (input.Contains(item.Id.Substring(3, item.Id.Length - 4)))

                    {
                        /* Logs the accepted acces to the log database with the 
                         time stamp*/
                        logWrite("FILE ACCESSED: " +
                            DateTime.Now.ToString("HH:mm:ss tt") + " --- PATH: " +
                            item.Id.Substring(3, item.Id.Length - 4));

                        /* Set permission level of the output parameter */
                        perm.PermissionLevel = (ushort)(item.Id[1] - '0');
                        return Constants.SUCCESS_FOUND;
                    }
                }
            }
            perm.PermissionLevel = Constants.COMPLETE_ACCESS;
            return Constants.SUCCESS_NOT_FOUND;
        }


        /* ARGS: NONE
         * RETURNS: Pointer to the connected filter port 
         * Connects app to the filter via port 
         */
        unsafe static System.IntPtr connectFilter()
        {
            System.Console.WriteLine("Scanner: Connecting to the filter ...");
            System.String FilterPortName = "\\ScannerPort";

            /* Connect to the server port -if exists- */
            System.IntPtr port = IntPtr.Zero;

            SECURITY_ATTRIBUTES sec_at = new SECURITY_ATTRIBUTES();

            IntPtr pnt = Marshal.AllocHGlobal(Marshal.SizeOf(sec_at));

            Marshal.StructureToPtr(sec_at, pnt, false);

            SECURITY_ATTRIBUTES sec_at2 =
                (SECURITY_ATTRIBUTES)Marshal.PtrToStructure(pnt, typeof(SECURITY_ATTRIBUTES));

            System.Int32 hr = FilterConnectCommunicationPort(
                FilterPortName,
                0,
                IntPtr.Zero,
                0,
                null,
                &port
            );

            /* Connection failed */
            if (hr != 0)
            {
                System.Console.WriteLine($"ERROR: {hr}");
            }
            System.Console.WriteLine($"Connected to {port}");

            return port;
        }

        /* ARGS: Reference to the pointer to the port that the 
        *   filter is connected to
        * RETURNS: NONE
        * Main logic that listens and replies to the requests that
        *   are sent by the filter
        */
        unsafe static void listenFilter(ref System.IntPtr port)
        {
            /* Prepare a buffer for the incoming message */
            JOE message;
            System.UInt16 buf = 1024;
            message = new JOE();

            /* Prepare a buffer for sending the reply */
            _REPLY_STRUCTURE replyMessage;
            replyMessage = new _REPLY_STRUCTURE();


            string mesBuff = "";
            while (true)
            {
                System.Console.WriteLine("Listening...");

                /* Get the kernel message */
                FilterGetMessage(port, &(message.header), buf, null);

                byte* ptr = (byte*)message.message.buffer;
                for (; *ptr != 0; ptr++)
                {
                    mesBuff += (char)*ptr;
                }

                //System.Console.WriteLine($"mesBuff: {mesBuff}");
                /* config the reply according to the sent message */
                replyMessage.header.MessageId = message.header.messageId;

                /* Check and Update the perm */
                int exit = CheckPerm(mesBuff, ref replyMessage.reply.permission);

                /* Reply to the sent message */
                System.Int32 res = FilterReplyMessage(
                    port,
                    &replyMessage.header,
                    (System.UInt32)Marshal.SizeOf(typeof(_REPLY_STRUCTURE))
                );

                /* For debugging */
                System.Console.WriteLine($"status{res}");
                mesBuff = "";
            }
        }

        unsafe static void Main(string[] args)
        {
            /* Permission File Init for debugging only */
            permWrite(":0:bash.txt;");
            permWrite(":1:random.txt;");
            permWrite(":5:writable.txt;");
            permWrite(":7:complete.txt;");
            permReadAll();

            System.IntPtr port = connectFilter();
            listenFilter(ref port);
            return;
        }
    }

}
