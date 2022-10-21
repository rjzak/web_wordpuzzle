#!env python3

from pathlib import Path

PACKAGE = "net.wordpuzzle.assets"
ASSETS = ( ("jquery", open("../../assets/jquery-3.6.0.min.js", "rb").read()), ("board", open("../../assets/board.html", "rb").read()), ("wordlist", open("../../assets/wordList.txt", "rb").read()), )

def divide_chunks(l, n):
    # https://www.geeksforgeeks.org/break-list-chunks-size-n-python/
    # looping till length l
    for i in range(0, len(l), n):
        yield l[i:i + n]

package_path = "/".join(PACKAGE.split("."))
for asset in ASSETS:
    name = asset[0]
    data = asset[1]
    
    Path("src/main/java/%s" % package_path).mkdir(parents=True, exist_ok=True)
    
    output_file = open("src/main/java/%s/%s.java" % (package_path, name.title()), "w")
    output_file.write("// Generated file, do not edit!\n")
    output_file.write("package %s;\n\n" % PACKAGE)

    if name == "jquery":
        output_file.write("import java.util.List;\n")
        output_file.write("import java.util.ArrayList;\n\n")

    output_file.write("public class %s {\n" % name.title())
    
    if name == "jquery":
        # JQuery is too large for Java to handle in one array
        final_index = 0
        output_file.write("\tprivate final static List<Byte> jquery = new ArrayList<>();\n\n")

        output_file.write("\tpublic static byte[] jquery() {\n")
        output_file.write("\t\tbyte[] bytes = new byte[jquery.size()];\n")
        output_file.write("\t\tint counter = 0;\n")
        output_file.write("\t\tfor (Byte b: jquery) {\n")
        output_file.write("\t\t\tbytes[counter++] = (byte) b;\n")
        output_file.write("\t\t}\n")
        output_file.write("\t\treturn bytes;\n")
        output_file.write("\t}\n\n")

        for index, part in enumerate(divide_chunks(data, 30)):
            output_file.write("\tprivate static void do_it_%d() {\n" % index)
            for p2 in part:
                output_file.write("\t\tjquery.add((byte) 0x%02x);\n" % p2)
            output_file.write("\t}\n\n")
            final_index = index
        output_file.write("\tstatic {\n")
        for index in range(final_index+1):
            output_file.write("\t\tdo_it_%d();\n" % index)
        output_file.write("\t}\n")
    elif name == "board":
        output_file.write("\tpublic static final byte[] %s = {" % name.upper())
        output_file.write(",".join(["(byte)0x%02X" % x for x in data]))
        output_file.write("};\n")
    else:
        output_file.write("\tpublic static final char[] %s= {" % name.upper())
        output_file.write(",".join(["0x%02X" % x for x in data]))
        output_file.write("};\n")

    if name == "wordlist":
        output_file.write("\tpublic static final char DELIMITER = 0x0A;\n")
        output_file.write("\tpublic static final int WORD_SIZE = 5;\n")
    
    output_file.write("}\n")
    output_file.close()

# vim: tabstop=8 expandtab shiftwidth=4 softtabstop=4
