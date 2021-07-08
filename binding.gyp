{
  "targets": [
    {
      "target_name": "node-vl53l3cx",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [
        "./src/vl53l3cx.cpp",
        "./src/vl53l3cxWorker.cpp",
        "./src/vl53lx_class.cpp"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
    }
  ]
}