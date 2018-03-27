//          Copyright Jean Pierre Cimalando 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <llvm/Object/COFF.h>
#include <getopt.h>
namespace obj = llvm::object;

static void show_usage()
{
    llvm::errs() <<
        "dlldepends [options] <files...>\n" <<
        "Print DLL dependencies of a Windows binary.\n" <<
        "  -i      print entries of the regular import directory\n"
        "  -d      print entries of the delayed import directory\n"
        "  -0      separate results with null bytes\n";
}

namespace arg {
static char linesep = '\n';
static bool show_imports = false;
static bool show_delayimports = false;
}

static std::error_code process_file(const char *filename)
{
    std::error_code ec;

    llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> e_mb =
        llvm::MemoryBuffer::getFileOrSTDIN(filename);
    if (!e_mb)
        return e_mb.getError();

    obj::COFFObjectFile obj(**e_mb, ec);
    if (ec)
        return ec;

    char linesep = arg::linesep;
    if (arg::show_imports) {
        for (obj::ImportDirectoryEntryRef dir : obj.import_directories()) {
            llvm::StringRef name;
            if ((ec = dir.getName(name)))
                return ec;
            llvm::outs() << name << linesep;
        }
    }
    if (arg::show_delayimports) {
        for (obj::DelayImportDirectoryEntryRef dir : obj.delay_import_directories()) {
            llvm::StringRef name;
            if ((ec = dir.getName(name)))
                return ec;
            llvm::outs() << name << linesep;
        }
    }

    return ec;
}

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        show_usage();
        return 0;
    }

    for (int c; (c = getopt(argc, argv, "0id")) != -1;) {
        switch (c) {
        case '0':
            arg::linesep = '\0'; break;
        case 'i':
            arg::show_imports = true; break;
        case 'd':
            arg::show_delayimports = true; break;
        default:
            show_usage(); return 1;
        }
    }
    if (!arg::show_imports && !arg::show_delayimports) {
        llvm::errs() << "Please specify at least one of [-i|-d].\n";
        return 1;
    }

    for (int i = optind; i < argc; ++i) {
        const char *filename = argv[i];
        std::error_code ec = process_file(filename);
        if (ec) {
            llvm::errs() << ec.message() << '\n';
            return 1;
        }
    }

    return 0;
}
