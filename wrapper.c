#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

HANDLE child_proc = INVALID_HANDLE_VALUE;

static void intHan(int signum) {
    if (child_proc != INVALID_HANDLE_VALUE) {
        TerminateProcess(child_proc, 0);
    }
}

int main(int argc, char *argv[], char *envp[]) {
    if (signal(SIGINT, intHan) == SIG_ERR) {
        perror("signal");
        return 1;
    }

    // Change directory to rootfs
    if (!SetCurrentDirectory("./rootfs")) {
        fprintf(stderr, "SetCurrentDirectory failed (%d)\n", GetLastError());
        return 1;
    }

    // Create required directories
    CreateDirectory("data\\data\\com.apple.android.music\\files", NULL);
    CreateDirectory("data\\data\\com.apple.android.music\\files\\mpl_db", NULL);

    // Create process
    STARTUPINFO si = {sizeof(si)};
    PROCESS_INFORMATION pi;

    char cmdline[MAX_PATH];
    snprintf(cmdline, sizeof(cmdline), "system\\bin\\main.exe %s", GetCommandLine() + strlen(argv[0]));

    if (!CreateProcess(NULL, cmdline,
        NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        fprintf(stderr, "CreateProcess failed (%d)\n", GetLastError());
        return 1;
    }

    child_proc = pi.hProcess;
    CloseHandle(pi.hThread);

    // Wait for child process
    WaitForSingleObject(child_proc, INFINITE);
    
    DWORD exitCode;
    GetExitCodeProcess(child_proc, &exitCode);
    CloseHandle(child_proc);

    return exitCode;
}
