using Microsoft.Speech.Recognition;
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
        public static int port_1 = 2081;
        public static int port_2 = 2063;
        public static int count_t = 0;
        public static TcpClient client = new TcpClient();

        public static void worker2()
        {
            client = new TcpClient("192.168.0.100", port_2);
            SpeechSynthesizer speaker2 = new SpeechSynthesizer();
            speaker2.SetOutputToDefaultAudioDevice();

            Console.WriteLine("-------------------------------------------------------------------Thread2 Started");
            try
            {
                Stream s = client.GetStream();
                StreamReader sr = new StreamReader(s);
                StreamWriter sw = new StreamWriter(s);
                sw.AutoFlush = true;
                //while (true)
                {
                    Console.WriteLine("===========================================================Stuck at Reading");
                    f_c = sr.ReadLine();
                    Console.WriteLine("Read! Face count is " + f_c);
                    speaker2.Speak("I see " + f_c + " faces");
                }

                Console.WriteLine("Die");
                s.Close();
            }
            finally
            {
                client.Close();
            }
        }

        public static void worker()
        {
            Program prog = new Program();
            TcpClient client1 = new TcpClient();
            client1 = new TcpClient("192.168.0.100", port_1);

            Console.WriteLine("Thread Started");
            try
            {
                Stream s = client1.GetStream();
                StreamReader sr = new StreamReader(s);
                StreamWriter sw = new StreamWriter(s);
                sw.AutoFlush = true;

                while (true)
                {
                    sw.WriteLine(data.ToString());
                }

                s.Close();
            }
            finally
            {
                client1.Close();
            }
        }

        static void Main(string[] args)
        {
            SpeechRecognitionEngine sre = new SpeechRecognitionEngine(new System.Globalization.CultureInfo("en-US"));
            sre.SetInputToDefaultAudioDevice();
            Choices commands = new Choices();
            commands.Add(new string[] { "hello", "hey", "hi", "how are you", "kese ho", "what is your name", "tell me about yourself", "who has programmed you", "nice to meet you", "tell me about your features", "pattern follow", "face count", "face follow", "chotu wake up", "chotu sleep", "chotu forward", "chotu backward", "chotu right", "chotu left", "chotu stop", "bus ruk jao" });

            GrammarBuilder gb = new GrammarBuilder();
            gb.Append(commands);

            // Create the Grammar instance.
            Grammar g = new Grammar(gb);
            sre.LoadGrammar(g);

            //Thread
            ThreadStart childref = new ThreadStart(worker);
            Console.WriteLine("In Main: Creating the Child thread");
            Thread childThread = new Thread(childref);
            childThread.Start();
            //Thread

            sre.SpeechRecognized += new EventHandler<SpeechRecognizedEventArgs>(sre_SpeechRecognized);
            //            sre.SpeechRecognitionRejected += new EventHandler<SpeechRecognitionRejectedEventArgs>(sre_SpeechUnrecognized);

            while (true)
            {
                Console.WriteLine("listening");
                sre.Recognize();
            }
        }

        static void sre_SpeechRecognized(object sender, SpeechRecognizedEventArgs e)
        {
            //Speaker
            SpeechSynthesizer speaker = new SpeechSynthesizer();
            speaker.SetOutputToDefaultAudioDevice();

            Console.WriteLine("Matched");
            float conf = e.Result.Confidence;
            int c;

            //General Communication
            c = string.Compare(e.Result.Text.ToString(), "hello");
            if (c == 0)
            {
                speaker.Speak("Hello. Welcome to Connexions 2014");
            }

            c = string.Compare(e.Result.Text.ToString(), "hey");
            if (c == 0)
            {
                speaker.Speak("Hey");
            }

            c = string.Compare(e.Result.Text.ToString(), "hi");
            if (c == 0)
            {
                speaker.Speak("Hi!");
            }

            c = string.Compare(e.Result.Text.ToString(), "how are you");
            if (c == 0)
            {
                speaker.Speak("I am good. How are you");
            }

            c = string.Compare(e.Result.Text.ToString(), "kese ho");
            if (c == 0)
            {
                speaker.Speak("I am good. How are you");
            }

            c = string.Compare(e.Result.Text.ToString(), "what is your name");
            if (c == 0)
            {
                speaker.Speak("My name is TurtleBot. But you can call me Chhutu");
            }

            c = string.Compare(e.Result.Text.ToString(), "who has programmed you");
            if (c == 0)
            {
                speaker.Speak("Imran, Nabeel and Safeer. has programmed me");
            }

            c = string.Compare(e.Result.Text.ToString(), "tell me about yourself");
            if (c == 0)
            {
                speaker.Speak("I am Turtlebot. I am running on ROS. I participated in Eraan Open 2014 Competition. I met with other robots there");
            }

            c = string.Compare(e.Result.Text.ToString(), "nice to meet you");
            if (c == 0)
            {
                speaker.Speak("Thank you. Same here");
            }

            c = string.Compare(e.Result.Text.ToString(), "tell me about your features");
            if (c == 0)
            {
                speaker.Speak("My features are: I can talk. I can recognise faces. I can recognise colors. I can learn any path");
            }

            //General Communication Ends

            //------------------------------------------//

            //Commands
            c = string.Compare(e.Result.Text.ToString(), "chotu wake up");
            if (c == 0)
            {
                speaker.Speak("Hi! I am Listening");
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

                c = string.Compare(e.Result.Text.ToString(), "bus ruk jao");
                if (c == 0)
                {
                    speaker.Speak("Ok!");
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

                    //if (count_t == 0)
                    {
                        //Thread
                        ThreadStart childref2 = new ThreadStart(worker2);
                        Thread childThread2 = new Thread(childref2);
                        childThread2.Start();
                        //Thread
                    }
                    count_t++;
                }
                c = string.Compare(e.Result.Text.ToString(), "face follow");
                if (c == 0)
                {
                    speaker.Speak("Following");
                    data = 9;
                }

                c = string.Compare(e.Result.Text.ToString(), "pattern follow");
                if (c == 0)
                {
                    speaker.Speak("Following");
                    data = 10;
                }
            }

        }
    }
}