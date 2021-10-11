#include "main.h"
#include "console.h"
#include "lua.h"

RemoteDebug Debug;

Task CONSOLE_task(0, TASK_FOREVER, &console_task_callback);


void console_task_callback (void) {
    Debug.handle();
}


void funcArgStr (String str) {

	debugA("*** called with arg.: %s", str.c_str());
}
void funcArgChar (char character) {

	debugA("*** called with arg.: %c", character);
}
void funcArgInt (int number) {

	debugA("*** called with arg.: %d", number);
}

void processCmdRemoteDebug() {

	String lastCmd = Debug.getLastCommand();

	if (lastCmd == "memory") {
    	mem_stat();
	}
}


void init_console(Scheduler *sc) {

    MDNS.addService("telnet", "tcp", 23);// Telnet server RemoteDebug
    Debug.begin(WiFiSettings.hostname.c_str()); // Initialize the WiFi server
	//Debug.setPassword(WiFiSettings.password.c_str()); // Password on telnet connection ?
    Debug.setResetCmdEnabled(true); // Enable the reset command
	Debug.showProfiler(true); // Profiler (Good to measure times, to optimize codes)
	Debug.showColors(true); // Colors
    Debug.setSerialEnabled(true);
    Debug.setCallBackProjectCmds(&processCmdRemoteDebug);
    Debug.initDebugger(debugGetDebuggerEnabled, debugHandleDebugger, debugGetHelpDebugger, debugProcessCmdDebugger); // Set the callbacks
    debugInitDebugger(&Debug); // Init the debugger
    if (debugAddFunctionInt("funcArgInt", &funcArgInt) >= 0) {
        	debugSetLastFunctionDescription("To run with Integer arg");
    }
    sc->addTask(CONSOLE_task);
    CONSOLE_task.enable();
}