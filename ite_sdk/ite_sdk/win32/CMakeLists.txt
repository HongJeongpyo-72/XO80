file(GLOB files ${CMAKE_CURRENT_SOURCE_DIR}/include/sys/*.in)

foreach (src ${files})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/include/sys/" "${CMAKE_CURRENT_BINARY_DIR}/include/sys/" tmp ${src})
    string(REPLACE ".in" "" dest ${tmp})
    configure_file(${src} ${dest})
endforeach()

file(GLOB files ${CMAKE_CURRENT_SOURCE_DIR}/include/*.in)

foreach (src ${files})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/include/" "${CMAKE_CURRENT_BINARY_DIR}/include/" tmp ${src})
    string(REPLACE ".in" "" dest ${tmp})
    configure_file(${src} ${dest})
endforeach()

ITE_ADD_ALL_SUBDIRECTORIES_EXCEPT(include lib)
