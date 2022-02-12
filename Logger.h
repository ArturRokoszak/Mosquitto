#include <sstream>
#include <string>
#include <syslog.h>

class Logger
{
public:
   static void Initialize(const std::string& appName)
   {
      setlogmask(LOG_UPTO (LOG_NOTICE));
      openlog(appName.c_str(), LOG_PID | LOG_NDELAY, LOG_LOCAL1);
      Log(appName.c_str(), "Logger Initialized...");
   }

   template <class T>
   static void Log(std::stringstream& ss, T&& t)
   {
      ss << ' ' << t;
   }

   template <class T, class... Args>
   static void Log(std::stringstream& ss, T&& t, Args&&... args)
   {
      Log(ss, t);
      Log(ss, args...);
   }

   template <class... Args>
   static void Log(Args&&... args)
   {
      std::stringstream ss;
      Log(ss, args...);
      syslog(LOG_NOTICE, "%s", ss.str().c_str());
   }

   static void Deinit()
   {
       closelog();
   }
};
