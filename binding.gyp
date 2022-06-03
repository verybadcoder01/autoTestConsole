{
    "targets": [{
        "target_name": "testaddon",
        "cflags!": [ "-fno-exceptions", "-std=c++20" ],
        "cflags_cc!": [ "-fno-exceptions", "-std=c++20" ],
        "sources": [
            "connector.cpp"
        ],
        'include_dirs': [
            "<!@(node -p \"require('node-addon-api').include_dir\")"
        ],
        'libraries': [],
        'dependencies': [
            "<!(node -p \"require('node-addon-api').gyp\")"
        ],
        'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
    }]
}