#ifndef UTILS_H
#define UTILS_H

#include "crypto_core.h"

// Prompt the user for a key (input is masked with '*'). Validates minimum length.
std::string readKey(const char* prompt);

// Ask "File exists. Overwrite? (y/n)" and return true if user says yes.
bool confirmOverwrite(const std::string& filepath);

#endif // UTILS_H
