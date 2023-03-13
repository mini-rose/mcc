Parser
======

The parser's job is to take an array of tokens produced by the tokenizer and
turn them into an abstract syntax tree (AST). You can easily see the generated
tree with this example::

        fn main {
                x: i32
        }

Then compile the test.mo file with the `-Xtree` option, which will dump the
generated AST before continuing to the emitting step::

        $ mcc -Xtree test.mo
        module src=`test.mo`
          fn-decl main()
          fn-def for `main`
            var-decl `x`: i32

As you can see, the fn-decl and fn-def are for some reason two seperate nodes.
This is done for 2 things: when importing modules, we only want to include the
declarations, treating a file as a "header" for building single .o objects. In
the case that we want to import & parse everything as a standalone executable,
then we can go back and parse the definitions. The second reason allows the
parser to "hoist" functions before their implementations, meaning you can place
your function in the source file above or below the declaration of another used
function, allowing for easier programming.

In another example, we can see that the functions declarations get grouped at
the top::

        module src=`example.mo`
          fn-decl other(a: i32, b: &i32) -> i32         # here
          fn-decl main()                                # and here
          fn-def for `other`
            var-decl `c`: &i32
            var-decl `d`: i32
          fn-def for `main`

This works because internally, the fn-decl node holds a `block_start` pointer
to the token of '{', where the function definition starts. After processing all
declarations, the parser returns to the module node and looks for any fn-decl
nodes in the top-level child list, calling p_parse_fn_def on that node.
