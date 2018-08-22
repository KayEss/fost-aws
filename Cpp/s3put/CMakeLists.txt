add_executable(s3put
        s3put.cpp
    )
target_link_libraries(s3put fost-cli fost-s3)
install(TARGETS s3put EXPORT ftest RUNTIME DESTINATION bin)
