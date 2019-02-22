{
    "targets": [
        {
            "target_name": "win-certs",
            "cflags!": [
                "-fno-exceptions"
            ],
            "cflags_cc!": [
                "-fno-exceptions"
            ],
            "msvs_settings": {
                "VCCLCompilerTool": {
                    "ExceptionHandling": 1
                }
            },
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")"
            ],
            "dependencies": [
                "<!(node -p \"require('node-addon-api').gyp\")"
            ],
            "conditions": [
                [
                    "OS=='win'",
                    {
                        "sources": [
                            "napi/module.cpp"
                        ],
                        "link_settings": {
                            "libraries": [
                                "-lcrypt32"
                            ]
                        }
                    }
                ]
            ]
        }
    ]
}