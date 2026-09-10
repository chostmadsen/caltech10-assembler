syntax clear


"-NUMBERS--------------------------------------------------------------------------------------------------------------"
syntax match    citobj_hex      "\v<[0-9a-fA-F]+>"

"-COMMENTS-------------------------------------------------------------------------------------------------------------"
syntax keyword  citobj_todo     contained TODO
syntax match    citobj_comment  ";.*"                                   contains=citobj_todo


"-LINKING--------------------------------------------------------------------------------------------------------------"
highlight default link citobj_hex       Identifier
highlight default link citobj_comment   Comment
highlight default link citobj_todo      Todo


let b:current_syntax    =   "citobj"
