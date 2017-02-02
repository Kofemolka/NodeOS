using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.ServiceProcess;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Silencer
{
    public partial class Silencer : ServiceBase
    {
        [DllImport("Powrprof.dll", CharSet = CharSet.Auto, ExactSpelling = true)]
        public static extern bool SetSuspendState(bool hiberate, bool forceCritical, bool disableWakeEvent);

        const string logSource = "Silencer";
        public Silencer()
        {
            InitializeComponent();         

            if (!EventLog.SourceExists(logSource))
                EventLog.CreateEventSource(logSource, "Application");
        }

        protected override void OnStart(string[] args)
        {
            _listener = Task.Factory.StartNew(() =>
           {
               UdpClient udpServer = new UdpClient(9);

               while (!tokenSource.Token.IsCancellationRequested)
               {
                   var remoteEP = new IPEndPoint(IPAddress.Any, 9);
                   var data = udpServer.Receive(ref remoteEP);
                   EventLog.WriteEntry(logSource, "receive data from " + remoteEP.ToString());

                   string stringData = Encoding.ASCII.GetString(data);
                   EventLog.WriteEntry(logSource, stringData);

                   if(stringData == "GoToSlEeP")
                   {
                       EventLog.WriteEntry(logSource, "Going to Sleep");
                       SetSuspendState(false, true, true);                       
                   }
               }
           }, tokenSource.Token);
        }

        protected override void OnStop()
        {
            tokenSource.Cancel();
        }

        private Task _listener;
        private CancellationTokenSource tokenSource = new CancellationTokenSource();        
    }
}
