add_library(fost-aws
        fost-aws.cpp
        s3.cpp
        s3authen.cpp
    )
target_include_directories(fost-aws PUBLIC ../include)
target_link_libraries(fost-aws fost-inet)
install(TARGETS fost-aws LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

add_library(fost-aws-smoke STATIC EXCLUDE_FROM_ALL
        s3authen-tests.cpp
    )
target_link_libraries(fost-aws-smoke fost-aws)
smoke_test(fost-aws-smoke)
