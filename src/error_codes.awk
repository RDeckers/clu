#TODO match a comment which contains "[Ee]rror [Cc]odes" or doesn't
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
        s = s"    case "$3":\n      return \""$2"\";\n"
    }
}

END {
 print "#include <clu.h>\n\nconst char* cluErrorString(cl_int err){\n  switch(err){\n"s"  }\n  return \"CLU_UNKNOWN_ERROR\";\n}";
}
