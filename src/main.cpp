#include <iostream>
#include <fstream>
#include <exception>

#include <libssh/libsshpp.hpp>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

#include <inotify-cpp/NotifierBuilder.h>

using namespace inotify;

int main(int argc, char** argv) {

    po::options_description description("Mirror sync usage");

    description.add_options()
            ("help", "Shows the help")
            ("port,p", po::value<short>(), "Specifies the ssh port")
            ("folder,f", po::value<std::string>(), "The folder to sync")
            ("host,h", po::value<std::string>(), "The ssh host")
            ("user,u", po::value<std::string>(), "The ssh user")
            ("role,r", po::value<std::string>(), R"(Specifies the role of "client" or "server")");

    po::variables_map vm;

    try {
        po::store(po::command_line_parser(argc, argv).options(description).run(), vm);
        po::notify(vm);
    } catch (std::exception &e) {
        std::cout << description;
        return 0;
    }

    if(vm.count("help") || !vm.count("role")){
        std::cout << description;
        return 0;
    }

    auto role = vm["role"].as<std::string>();

    if (role == "server" && (!vm.count("folder") || vm.count("port") || vm.count("host") || vm.count("user"))) {
        std::cout << "please provide the correct input" << std::endl;
        std::cout << description;
        return 0;
    }

    if (role == "client" && (!vm.count("folder") || !vm.count("port") || !vm.count("host") || !vm.count("user"))) {
        std::cout << "please provide the correct input" << std::endl;
        std::cout << description;
        return 0;
    }

    if (role == "client") {
        //Client code
        /*ssh::Session session;
        session.setOption(SSH_OPTIONS_HOST, vm["host"].as<std::string>().c_str());
        session.setOption(SSH_OPTIONS_USER, vm["user"].as<std::string>().c_str());
        session.setOption(SSH_OPTIONS_PORT, vm["port"].as<short>());*/

        ssh_session session;
        int rc;
        session = ssh_new();
        if (session == NULL)
            exit(-1);
        ssh_options_set(session, SSH_OPTIONS_HOST, "localhost");
        ssh_options_set(session, SSH_OPTIONS_PORT_STR, "2222");
        ssh_options_set(session, SSH_OPTIONS_USER, "valde");
        rc = ssh_connect(session);
        if (rc != SSH_OK)
        {
            fprintf(stderr, "Error connecting to localhost: %s\n",
                    ssh_get_error(session));
            exit(-1);
        }

        rc = ssh_userauth_kbdint(session, NULL, NULL);
        while (rc == SSH_AUTH_INFO)
        {
            const char *name, *instruction;
            int nprompts, iprompt;
            name = ssh_userauth_kbdint_getname(session);
            instruction = ssh_userauth_kbdint_getinstruction(session);
            nprompts = ssh_userauth_kbdint_getnprompts(session);
            if (strlen(name) > 0)
                printf("%s\n", name);
            if (strlen(instruction) > 0)
                printf("%s\n", instruction);
            for (iprompt = 0; iprompt < nprompts; iprompt++)
            {
                const char *prompt;
                char echo;
                prompt = ssh_userauth_kbdint_getprompt(session, iprompt, &echo);
                if (echo)
                {
                    char buffer[128], *ptr;
                    printf("%s", prompt);
                    if (fgets(buffer, sizeof(buffer), stdin) == NULL)
                        return SSH_AUTH_ERROR;
                    buffer[sizeof(buffer) - 1] = '\0';
                    if ((ptr = strchr(buffer, '\n')) != NULL)
                        *ptr = '\0';
                    if (ssh_userauth_kbdint_setanswer(session, iprompt, buffer) < 0)
                        return SSH_AUTH_ERROR;
                    memset(buffer, 0, strlen(buffer));
                }
                else
                {
                    char *ptr;
                    ptr = getpass(prompt);
                    if (ssh_userauth_kbdint_setanswer(session, iprompt, ptr) < 0)
                        return SSH_AUTH_ERROR;
                }
            }
            rc = ssh_userauth_kbdint(session, NULL, NULL);
        }
    } else {
        //Server code

    }

    return 0;
}