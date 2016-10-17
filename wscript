# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def build(bld):
    module = bld.create_ns3_module('o-icn', ['internet', 'network', 'core', 'applications'])
    module.source = [
        'model/oicn-header.cc',
        'model/content.cc',
        'model/icn-manager.cc',
        'model/named-content-cache-entry.cc',
        'model/named-content-cache.cc',
        'model/cache-with-fifo.cc',
        'model/cache-with-lru.cc',
        'model/cache-with-lfu.cc',        
        'model/simple-universal-caching.cc',
        'model/cache-manager.cc',
        'model/oicn-server-application.cc',
        'model/oicn-client-application.cc',
        'model/oicn-zipf-client.cc',
        'model/sublayer-protocol.cc',
        'model/ipv4-router-l3-protocol.cc',
        'model/dnsplus-header.cc',
        'model/dnsplus-question-header.cc',
        'model/dnsplus-answer-section.cc',
        'model/icn-router.cc',        
        'helper/router-stack-helper.cc',
        'helper/oicn-application-helper.cc',
        
        ]

    module_test = bld.create_ns3_module_test_library('o-icn')
    module_test.source = [
        'test/o-icn-test-suite.cc',
        ]

    headers = bld(features='ns3header')
    headers.module = 'o-icn'
    headers.source = [
        'model/oicn-header.h',
        'model/content.h',
        'model/icn-manager.h',
        'model/named-content-cache-entry.h',
        'model/named-content-cache.h',
        'model/cache-with-fifo.h',
        'model/cache-with-lru.h',
        'model/cache-with-lfu.h',
        'model/simple-universal-caching.h',
        'model/cache-manager.h',
        'model/oicn-server-application.h',
        'model/oicn-client-application.h',
        'model/oicn-zipf-client.h',
        'model/sublayer-protocol.h',
        'model/ipv4-router-l3-protocol.h',
        'model/dnsplus-header.h',
        'model/dnsplus-question-header.h',
        'model/dnsplus-answer-section.h',
        'model/icn-router.h',        
        'helper/router-stack-helper.h',
        'helper/oicn-application-helper.h',

        ]

    if bld.env.ENABLE_EXAMPLES:
        bld.recurse('examples')

    bld.ns3_python_bindings()

