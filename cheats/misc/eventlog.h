#include <string>
#include <vector>

namespace Features
{
	struct EventLog_t
	{
		float time;
		int yOffset, alpha;
		std::string text;
	};

	class CEventLogger
	{
	public:
		void AddLog(const char* str, ...);
		void Draw_D3D9();
	private:
		std::vector<EventLog_t> logs;
	};
	extern CEventLogger* EventLogger;
}
extern void Game_Msg(const char* msg, ...);



