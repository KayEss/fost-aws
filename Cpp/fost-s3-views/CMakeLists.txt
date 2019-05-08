add_library(fost-s3-views
        proxy.cpp
    )
target_link_libraries(fost-s3-views fost-s3 fost-urlhandler)
set_target_properties(fost-s3-views PROPERTIES DEBUG_POSTFIX "-d")
install(TARGETS fost-s3-views LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
