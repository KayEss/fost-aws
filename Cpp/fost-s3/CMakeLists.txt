add_library(fost-s3
        fost-aws.cpp
        s3.cpp
        s3authen.cpp
    )
target_include_directories(fost-s3 PUBLIC ../include)
target_link_libraries(fost-s3 fost-inet)
set_target_properties(fost-s3 PROPERTIES DEBUG_POSTFIX "-d")
install(TARGETS fost-s3 LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

add_library(fost-s3-smoke STATIC EXCLUDE_FROM_ALL
        s3authen-tests.cpp
    )
target_link_libraries(fost-s3-smoke fost-s3)
smoke_test(fost-s3-smoke)
