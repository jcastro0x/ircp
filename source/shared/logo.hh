// This file is part of the IRCP Project. See LICENSE file for Copyright information

#pragma once

#define IRCP_LOGO \
" ██╗██████╗  ██████╗██████╗\n"\
" ██║██╔══██╗██╔════╝██╔══██╗\n"\
" ██║██████╔╝██║     ██████╔╝\n"\
" ██║██╔══██╗██║     ██╔═══╝\n"\
" ██║██║  ██║╚██████╗██║\n"\
" ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝\n"\
"Internet Relay Chat Plus\n"\
"\n"

#define PRINT_LOGO \
std::setlocale(LC_ALL, "en_US.UTF-8");\
std::cout << IRCP_LOGO;