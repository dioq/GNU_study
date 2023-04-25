# objcopy
[功能] 
将目标文件的一部分或者全部内容拷贝到另外一个目标文件中，或者实现目标文件的格式转换。 

[描述] 
objcopy工具使用BFD库读写目标文件，它可以将一个目标文件的内容拷贝到另外一个目标文件当中。objcopy通过它的选项来控制其不同的动作，它可以将目标文件拷贝成和原来的文件不一样的格式。需要注意的是objcopy能够在两种格式之间拷贝一个完全链接的文件，在两种格式之间拷贝一个可重定位的目标文件可能不会正常地工作。 
objcopy在做转换的时候会创建临时文件，然后将这些临时文件删除。objcopy使用BFD来做它所有的转换工作；它访问BFD中描述的所有格式，可以不必指定就识别大多数的格式。 
通过指定输出目标为srec(例如 -O srec)，objcopy可以用来生成S-record文件。 
通过指定输入目标为而进制文件(例如-O binary)，objcopy可以生成原始格式的二进制文件。当objcopy生成一个原始格式的二进制文件的时候，它会生成输入的目标文件的基本内存拷贝，然后所有的标号和可重定位信息都会被去掉。内存拷贝开始于最低段的加载地址，拷贝到输出文件。 
当生成一个S-record或者原始的二进制文件的时候，可以使用-S这个很有用的选项选项来移除一些包含调试信息的节。有时-R可以用来移除一些二进制文件不需要的节。 
注意：objcopy工具不能用来改变文件的大端和小端属性。 

命令参数： 
infile/outfile 
源文件/目标文件，如果不指定目标文件那么objcopy将会创建一个临时文件，并且将其命名为源文件。 

命令项： 
-I bfdname 
--input-target=bfdname 
    指定输入文件的bfdname,可取值elf32-little，elf32-big等。 

-O bfdname 
--output-target=bfdname 
    指定输出文件的bfdname 

-F bfdname 
--target=bfdname 
    指定输入、输出文件的bfdname，目标文件格式，只用于在目标和源之间传输数据，不转换。 

-j sectionname 
--only-section=sectionname 
    只将由sectionname指定的section拷贝到输出文件，可以多次指定，并且注意如果使用不当会导致输出文件不可用。 

-R sectionname 
--remove-section=sectionname 
    从输出文件中去除掉由sectionname指定的section，可以多次指定，并且注意如果使用不当会导致输出文件不可用。 

-S 
--strip-all 
    不从源文件拷贝符号信息和relocation信息。 

-g 
--strip-debug 
    不从源文件拷贝调试符号信息和相关的段。对使用-g编译生成的可执行文件执行之后，生成的结果几乎和不用-g进行编译生成可执行文件一样。 

--strip-unneeded 
    去掉所重定位处理不需要的符号。 

-K symbolname 
--keep-symbol=symbolname 
    strip的时候，保留由symbolname指定的符号信息。可以指定多次。 

-N symbolname 
--strip-symbol=symbolname 
    不拷贝由symbolname指定的符号信息，可以多次指定。 

-G symbolname 
--keep-global-symbol=symbolname 
    只保留symbolname为全局的，让其他的都是文件局部的变量这样外部不可见，这个选项可以多次指定。 

-L symbolname 
--localize-symbol=symbolname 
    将变量symbolname变成文件局部的变量。可以多次指定。 

-W symbolname 
--weaken-symbol=symbolname 
    弱化变量。 

--globalize-symbol=symbolname 
    让变量symbolname变成全局范围，这样它可以在定义它的文件外部可见。可以多次指定。 

-w 
--wildcard 
    允许对其他命令行项中的symbolnames使用正则表达式。问号(?)，星号(*)，反斜线(\)，和中括号([])操作可以用在标号名称的任何位置。如果标号的第一个字符是感叹号(!)，那么表示相反的含义，例如： 
    -w -W !foo -W fo* 
    表示objcopy将要弱化所有以"fo"开头的标号，但是除了标号"foo"。 

-x 
--discard-all 
    不从源文件中拷贝非全局变量。 

-X 
--discard-locals 
    不拷贝编译生成的局部变量(一般以L或者..开头)。 

-b byte 
--byte=byte 
    只保留输入文件的每个第byte个字节(不会影响头部数据)。byte的范围可以是0到interleave-1。这里，interleave通过-i选项指定，默认为4。将文件创建成程序rom的时候，这个命令很有用。它经常用于srec输出目标。 

-i interleave 
--interleave=interleave 
    每隔interleave字节拷贝1 byte。通过-b选项指定选择哪个字节，默认为4。如果不指定-b那么objcopy会忽略这个选项。 

--gap-fill val 
    在section之间的空隙中填充val, 

--set-start val 
    设定新文件的起始地址为val，并不是所有格式的目标文件都支持设置起始地址。 

--change-start incr 
--adjust-start incr 
    通过增加incr量来调整起始地址，并不是所有格式的目标文件都支持设置起始地址。 

--change-address incr 
--adjust-vma incr 
    通过增加incr量调整所有sections的VMA(virtual memory address)和LMA(linear memory address),以及起始地址。 

--change-section-address section{=,+,-}val 
--adjust-section-vma section{=,+,-}val 
    调整指定section的VMA/LMA地址。 

--set-section-flags section=flag 
    指定指定section的flag，flag的取值可以alloc，contents, load, noload, readonly, code, data, rom, share, debug。我们可以设置一个没有内容的节的flag，但是清除一个有内容的节的flag是没有意义的--应当把相应的节移除。并不是所有的flags对于所有的目标文件都有意义。 

--add-section sectionname=filename 
    在拷贝文件的时候，添加一个名为sectionname的section，该section的内容为filename的内容，大小为文件大小。这个选项只在那些可以支持任意名称section的文件好用。 

--rename-section oldname=newname[,flags] 
    更改section的名 
    将一个section的名字从oldname更改为newname，同时也可以指定更改其flags。这个在执行linker脚本进行重命名的时候，并且输出文件还是一个目标文件并不成为可执行的连接文件，这个时候很有优势。 
    这个项在输入文件是binary的时候很有用，因为这经常会创建一个名称为.data的section，例如，你想创建一个名称为.rodata的包含二进制数据的section，这时候，你可以使用如下命令： 
    objcopy -I binary -O <output_format> -B <architecture> \ 
    --rename-section .data=.rodata,alloc,load,readonly,data,contents \ 
    <input_binary_file> <output_object_file> 

--add-gnu-debuglink=path-to-file 
    创建一个.gnu_debuglink节，这个节包含一个特定路径的文件引用，并且把它添加到输出文件中。 

--only-keep-debug 
    对文件进行strip，移走所有不会被--strip-debug移走的section，并且保持调试相关的section原封不动。