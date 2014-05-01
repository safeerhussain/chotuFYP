using Microsoft.Speech.Recognition;
//using Microsoft.Speech.Synthesis;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Speech.Synthesis;
using System.Net;
using System.Threading;

namespace SpeechTest
{
    class Program
    {
        public int counter = 0;
        public static int data = 0;
        public static string f_c;
        public static int awake;

        public static void worker()
        {
            Program prog = new Program();
            TcpClient client = new TcpClient();
            client = new TcpClient("172.15.130.42", 2055);

            Console.WriteLine("Thread Started");
            try
            {
                Stream s = client.GetStream();
                StreamReader sr = new StreamReader(s);
                StreamWriter sw = new StreamWriter(s);
                sw.AutoFlush = true;
                Console.WriteLine(data.ToString());

                while (true)
                {
                    Console.WriteLine(data.ToString());
                    sw.WriteLine(data.ToString());
                    if (data == 8)
                    {
                        f_c = sr.ReadLine();
                        Console.WriteLine(f_c);
                    }
                }

                s.Close();
            }
            finally
            {
                client.Close();
            }
        }

        static void Main(string[] args)
        {
            SpeechRecognitionEngine sre = new SpeechRecognitionEngine(new System.Globalization.CultureInfo("en-US"));
            sre.SetInputToDefaultAudioDevice();
            Choices commands = new Choices();
            commands.Add(new string[] {"face count", "chotu wake up", "chotu sleep" , "chotu forward", "chotu backward", "chotu right", "chotu left", "chotu stop" });

            GrammarBuilder gb = new GrammarBuilder();
            gb.Append(commands);

            
            // Create the Grammar instance.
            Grammar g = new Grammar(gb);
            sre.LoadGrammar(g);

            Console.WriteLine("Here");
            ThreadStart childref = new ThreadStart(worker);
            Console.WriteLine("In Main: Creating the Child thread");
            Thread childThread = new Thread(childref);
            childThread.Start();
            Program prog = new Program();
            sre.SpeechRecognized += new EventHandler<SpeechRecognizedEventArgs>(sre_SpeechRecognized);
            while (true)
            {
                sre.Recognize();
            }
        }

        static void sre_SpeechRecognized(object sender, SpeechRecognizedEventArgs e)
        {
            SpeechSynthesizer speaker = new SpeechSynthesizer();
            speaker.SetOutputToDefaultAudioDevice();
           // speaker.Volume = 100;
            //speaker.SelectVoiceByHints(VoiceGender.Male, VoiceAge.Child);
           // speaker.Speak("Shuma Farsi Baladay");

            float conf = e.Result.Confidence;
            int c;
            c = string.Compare(e.Result.Text.ToString(), "chotu wake up");
            if (c == 0)
            {
                speaker.Speak("Hi! I am Awake and Listening");
                data = 6;
                awake = 1;
            }
            if (awake == 1)
            {
                c = string.Compare(e.Result.Text.ToString(), "chotu stop");
                if (c == 0)
                {
                    speaker.Speak("Stopping");
                    data = 1;
                }
                c = string.Compare(e.Result.Text.ToString(), "chotu forward");
                if (c == 0)
                {
                    data = 2;
                    speaker.Speak("Moving Forward");
                }
                c = string.Compare(e.Result.Text.ToString(), "chotu backward");
                if (c == 0)
                {
                    speaker.Speak("Moving Backward");
                    data = 3;
                }
                c = string.Compare(e.Result.Text.ToString(), "chotu right");
                if (c == 0)
                {
                    speaker.Speak("Turning Right");
                    data = 4;
                }
                c = string.Compare(e.Result.Text.ToString(), "chotu left");
                if (c == 0)
                {
                    speaker.Speak("Turning Left");
                    data = 5;
                }
                c = string.Compare(e.Result.Text.ToString(), "chotu sleep");
                if (c == 0)
                {
                    speaker.Speak("Ok. Sleeping now. Bye Bye");
                    data = 7;
                    awake = 0;
                }
                c = string.Compare(e.Result.Text.ToString(), "face count");
                if (c == 0)
                {
                    data = 8;
                    speaker.Speak("watching");
                    speaker.Speak("I see " + f_c + "faces");
                }
            }
            
        }
    }
}