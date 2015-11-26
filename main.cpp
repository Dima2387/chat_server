    #
    #include <cstring>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <sys/time.h>
    #include <netinet/in.h>
    #include <stdio.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <algorithm>
    #include <set>
    #include <iostream>
    #include <time.h>
    #include <stdlib.h>
    #include <getopt.h>
    #include <string.h>
    #include <string>
    #include <iostream>
    #include <stdio.h>
    #include <fstream>
    #include <unistd.h>
    #include <stdlib.h>
    #include  <thread>
    #include <fstream>
    #include <thread>


    //мои функции

    std::string vremya() { //вычисление текущего времени
    time_t rawtime;
    struct tm * timeinfo;
    char current_time[90];
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    strftime (current_time,90,"%F %H:%M:%S",timeinfo); //конец вычисления времени

    return current_time;
    }

    void to_log (const std::string &s){      // логгер
    std::ofstream fout("chat_server.html", std::ios_base::app | std::ios_base::ate);

        if (!fout.is_open()) // если файл небыл открыт
        {
         std::cout << "Не могу открыть файл\n"; // напечатать соответствующее сообщение
        }
    fout  << vremya() << " " << s << "<br>" << "\n";
    }

    void to_display (const std::string &s){
        std::cout << vremya() << " " << s <<"\n";
    }

    void to_all (const std::string &s) { //сделать запись в лог и вывести это же сообщение на экран
        to_display(s);
        to_log(s);
    }
    //мои функции кончились
    int main()
    {
       struct sockaddr_in addr;
       char buf[1024];
       int port = 3426;
       std::string version = "0.0.0.15.beta (26.11.2015)";
       std::string hello = " Сервер чата " + version + " запущен на порту "+ std::to_string(port);;
       int listener = socket(AF_INET, SOCK_STREAM, 0);
       std::string main_chat;

       if(listener < 0)
       {
           perror("socket");
           exit(1);
       }

       fcntl(listener, F_SETFL, O_NONBLOCK);

       addr.sin_family = AF_INET;
       addr.sin_port = htons(port);
       addr.sin_addr.s_addr = INADDR_ANY;
       if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
       {
           perror("bind");
           exit(2);
       }
       std::string encoding = "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">";
       std::string info = " Готов к обработке входящих соединений";

       std::ifstream my_file("chat_server.html");
       if (!my_file.good())
       {
           to_log(encoding);
         // read away
       }
       to_all(hello);
       to_all(info);

       listen(listener, 2);

       std::set<int> clients;
       clients.clear();

       while(true)
       {
           // Заполняем множество сокетов
           fd_set readset;
           FD_ZERO(&readset);
           FD_SET(listener, &readset);

           for (int sfd : clients) {
               FD_SET(sfd, &readset);
           }
           // Задаём таймаут
           timeval timeout;
           timeout.tv_sec = 1;
           timeout.tv_usec = 0;
           // Ждём события в одном из сокетов
           int mx = std::max(listener, *max_element(clients.begin(), clients.end()));
           if(select(mx+1, &readset, NULL, NULL, &timeout) < 0)
           {
               perror("select");
               exit(3);
           }
           // Определяем тип события и выполняем соответствующие действия
           if(FD_ISSET(listener, &readset))
           {
               // Поступил новый запрос на соединение, используем accept
               int sock = accept(listener, NULL, NULL);
               to_all("Обнаружено подключение");
               if(sock < 0)
               {
                   perror("accept");
                   exit(3);
               }
               fcntl(sock, F_SETFL, O_NONBLOCK);
               clients.insert(sock);
           }
           auto itr = clients.begin();
           while (itr != clients.end())
           {
               if(FD_ISSET(*itr, &readset))
               {
                   // Поступили данные от клиента, читаем их
                   memset(buf, 0, 1024);
                   int bytes_read = recv(*itr, buf, 1024, 0);

                   if(bytes_read <= 0)
                   {
                       // Соединение разорвано, удаляем сокет из множества
                       to_display("Клиент разорвал соединение");
                       to_log("Клиент разорвал соединение");
                       close(*itr);
                       clients.erase(itr++);
                       continue;
                   }
                    std::string otvet = " Получено сообщение: ";
                    otvet = otvet + buf;
                    main_chat += otvet;
                    to_display(buf);
                    to_log(buf);

                    int t = 1;
                    if (t > 0)
                    {
                        for (int sfd : clients) {
                            send(sfd, buf, 1024, 0);

                            t--;
                        }


                            }
               }
               ++itr;
           }
       }

       return 0;
    }
