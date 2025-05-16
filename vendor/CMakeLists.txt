project(AGVendor)

macro(fetch_dep dep repo tag)
    if (NOT TARGET ${dep})
        FetchContent_Declare(
                ${dep}
                GIT_REPOSITORY ${repo}
                GIT_TAG ${tag}
        )
        FetchContent_MakeAvailable(${dep})
    endif()
endmacro()

