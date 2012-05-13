# First-level heading

### 3rd-level heading

Once the plugin is built, you can run it using:

    $ clang <-g -Wall ...> -Xclang -load -Xclang /home/simshi/llvm/x86/lib/libPrintFunctionFanout.so -Xclang -add-plugin -Xclang func-fanout <input_file>
    $ clang <-g -Wall ...> -Xclang -load -Xclang /home/simshi/llvm/x86/lib/libPrintFunctionFanout.so -Xclang -plugin -Xclang func-fanout <input_file>
    $ clang <-g -Wall ...> -Xclang -load -Xclang /home/simshi/llvm/x86/lib/libPrintFunctionFanout.so -Xclang -add-plugin -Xclang func-fanout -Xclang -plugin-arg-func-fanout -Xclang help <input_file>


