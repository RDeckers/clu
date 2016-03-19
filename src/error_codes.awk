#Looks through (header) files for any line starting with "/*[something][Ee]rror [Cc]odes"
#From there it tracks all the "#define CL_SOMETHING number" error numbers
#It stops tracking when it reaches another line starting with "/*" which doesn't fit the expression above
#it then prints out a c file containing the function cluErrorString which converts each numerical error code
#into a string of it's defined name.
BEGIN {
    record=0
}

/^\/\*[a-zA-Z0-9 ]*[Ee]rror [Cc]odes/{
    record=1;
    next;
}

!/^\/\*[a-zA-Z0-9 ]*[Ee]rror [Cc]odes/ && /^\/\*/{
    record=0;
}

/#define CL_*/ {
    if (record==1) {
        #we use $3 here so that we don't have to bother with includes for the proper definitions.
        s = s"    case "$3":\n      return \""$2"\";\n"
    }
}

END {
 print "#include <clu.h>\n\nconst char* cluErrorString(cl_int err){\n  switch(err){\n"s"  }\n  return \"CLU_UNKNOWN_ERROR\";\n}";
}
