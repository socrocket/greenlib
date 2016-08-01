set(SourceFile "executable_output.txt")
file(READ ${SourceFile} Contents)

string(ASCII 27 Esc)
string(REGEX REPLACE "\n" "${Esc};" ContentsAsList "${Contents}")

unset(ModifiedContents)
foreach(Line ${ContentsAsList})
  if(NOT "${Line}" MATCHES "In file:")
    string(REGEX REPLACE "${Esc}" "\n" Line ${Line})
    set(ModifiedContents "${ModifiedContents}${Line}")
  endif()
endforeach()
file(WRITE ${SourceFile} ${ModifiedContents})
