add_executable(s3get
        s3get.cpp
    )
target_link_libraries(s3get fost-cli fost-s3)
install(TARGETS s3get EXPORT ftest RUNTIME DESTINATION bin)
