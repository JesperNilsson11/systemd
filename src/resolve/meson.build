# SPDX-License-Identifier: LGPL-2.1-or-later

resolve_includes = [includes, include_directories('.')]

basic_dns_sources = files(
        'resolved-dns-dnssec.c',
        'resolved-dns-packet.c',
        'resolved-dns-rr.c',
        'resolved-dns-answer.c',
        'resolved-dns-question.c',
        'resolved-util.c',
        'dns-type.c',
)

systemd_resolved_sources = files(
        'resolved-bus.c',
        'resolved-conf.c',
        'resolved-dns-cache.c',
        'resolved-dns-query.c',
        'resolved-dns-scope.c',
        'resolved-dns-search-domain.c',
        'resolved-dns-server.c',
        'resolved-dns-stream.c',
        'resolved-dns-stub.c',
        'resolved-dns-synthesize.c',
        'resolved-dns-transaction.c',
        'resolved-dns-trust-anchor.c',
        'resolved-dns-zone.c',
        'resolved-dnssd-bus.c',
        'resolved-dnssd.c',
        'resolved-etc-hosts.c',
        'resolved-link-bus.c',
        'resolved-link.c',
        'resolved-llmnr.c',
        'resolved-manager.c',
        'resolved-mdns.c',
        'resolved-resolv-conf.c',
        'resolved-socket-graveyard.c',
        'resolved-varlink.c',
)

resolvectl_sources = files(
        'resolvconf-compat.c',
        'resolvectl.c',
)

############################################################

dns_type_list_txt = custom_target(
        'dns_type-list.txt',
        input : ['generate-dns_type-list.sed', 'dns-type.h'],
        output : 'dns_type-list.txt',
        command : [sed, '-n', '-r', '-f', '@INPUT0@', '@INPUT1@'],
        capture : true)

generate_dns_type_gperf = find_program('generate-dns_type-gperf.py')

gperf_file = custom_target(
        'dns_type-from-name.gperf',
        input : dns_type_list_txt,
        output : 'dns_type-from-name.gperf',
        command : [generate_dns_type_gperf, 'dns_type', 'DNS_TYPE_', '@INPUT@'],
        capture : true)

dns_type_from_name_h = custom_target(
        'dns_type-from-name.h',
        input : gperf_file,
        output : 'dns_type-from-name.h',
        command : [gperf,
                   '-L', 'ANSI-C', '-t', '--ignore-case',
                   '-N', 'lookup_dns_type',
                   '-H', 'hash_dns_type_name',
                   '-p', '-C',
                   '@INPUT@'],
        capture : true)

dns_type_to_name_h = custom_target(
        'dns_type-to-name.h',
        input : ['dns_type-to-name.awk', dns_type_list_txt],
        output : 'dns_type-to-name.h',
        command : [awk, '-f', '@INPUT0@', '@INPUT1@'],
        capture : true)

generated_sources += [dns_type_from_name_h, dns_type_to_name_h]
basic_dns_sources += [dns_type_from_name_h, dns_type_to_name_h]

libsystemd_resolve_core = static_library(
        'systemd-resolve-core',
        basic_dns_sources,
        include_directories : includes,
        dependencies : userspace,
        build_by_default : false)

resolved_gperf_c = custom_target(
        'resolved_gperf.c',
        input : 'resolved-gperf.gperf',
        output : 'resolved-gperf.c',
        command : [gperf, '@INPUT@', '--output-file', '@OUTPUT@'])

resolved_dnssd_gperf_c = custom_target(
        'resolved_dnssd_gperf.c',
        input : 'resolved-dnssd-gperf.gperf',
        output : 'resolved-dnssd-gperf.c',
        command : [gperf, '@INPUT@', '--output-file', '@OUTPUT@'])

generated_sources += [resolved_gperf_c, resolved_dnssd_gperf_c]
systemd_resolved_sources += [resolved_gperf_c, resolved_dnssd_gperf_c]

systemd_resolved_dependencies = [threads, libm, libopenssl]
if conf.get('ENABLE_DNS_OVER_TLS') == 1
        systemd_resolved_sources += files(
                'resolved-dnstls.c',
        )
endif

link_with = [
        libshared,
        libsystemd_resolve_core,
]

resolve_common_template = {
        'link_with' : [
                libshared,
                libsystemd_resolve_core,
        ],
        'dependencies' : [
                libopenssl,
                libm,
        ],
}
resolve_test_template = test_template + resolve_common_template
resolve_fuzz_template = fuzz_template + resolve_common_template

executables += [
        libexec_template + {
                'name' : 'systemd-resolved',
                'dbus' : true,
                'conditions' : ['ENABLE_RESOLVE'],
                'sources' : systemd_resolved_sources +
                            files('resolved.c'),
                'include_directories' : resolve_includes,
                'link_with' : link_with,
                'dependencies' : systemd_resolved_dependencies,
        },
        executable_template + {
                'name' : 'resolvectl',
                'public' : true,
                'conditions' : ['ENABLE_RESOLVE'],
                'sources' : resolvectl_sources,
                'link_with' : link_with,
                'dependencies' : [
                        libopenssl,
                        libidn,
                        libm,
                        threads,
                ],
        },
        resolve_test_template + {
                'sources' : files('test-resolve-tables.c'),
        },
        resolve_test_template + {
                'sources' : files('test-dns-answer.c'),
        },
        resolve_test_template + {
                'sources' : files(
                        'test-dns-cache.c',
                        'resolved-dns-cache.c'
                ),
        },
        resolve_test_template + {
                'sources' : files('test-dns-packet.c'),
        },
        resolve_test_template + {
                'sources' : files('test-dns-packet-append.c'),
        },
        resolve_test_template + {
                'sources' : files('test-dns-packet-extract.c'),
        },
        resolve_test_template + {
                'sources' : files('test-dns-rr.c'),
        },
        resolve_test_template + {
                'sources' : files('test-dns-question.c'),
        },
        resolve_test_template + {
                'sources' : files(
                        'test-resolved-etc-hosts.c',
                        'resolved-etc-hosts.c',
                ),
        },
        resolve_test_template + {
                'sources' : files('test-resolved-packet.c'),
        },
        resolve_test_template + {
                'sources' : files('test-dnssec.c'),
                'conditions' : ['HAVE_OPENSSL'],
        },
        resolve_test_template + {
                'sources' : files('test-dnssec-complex.c'),
                'type' : 'manual',
        },
        test_template + {
                'sources' : [
                        files('test-dns-search-domain.c'),
                        basic_dns_sources,
                        systemd_resolved_sources,
                ],
                'dependencies' : systemd_resolved_dependencies,
                'include_directories' : resolve_includes,
        },
        test_template + {
                'sources' : [
                        files('test-dns-synthesize.c'),
                        basic_dns_sources,
                        systemd_resolved_sources,
                ],
                'dependencies' : [
                        systemd_resolved_dependencies,
                ],
                'include_directories' : resolve_includes,
        },
        test_template + {
                'sources' : [
                        files('test-dns-zone.c'),
                        basic_dns_sources,
                        systemd_resolved_sources,
                ],
                'dependencies' : [
                        systemd_resolved_dependencies,
                ],
                'include_directories' : resolve_includes,
        },
        test_template + {
                'sources' : [
                        files('test-resolved-link.c'),
                        basic_dns_sources,
                        systemd_resolved_sources,
                ],
                'dependencies' : [
                        systemd_resolved_dependencies,
                ],
                'include_directories' : resolve_includes,
        },
        test_template + {
                'sources' : [
                        files('test-dns-query.c'),
                        basic_dns_sources,
                        systemd_resolved_sources,
                ],
                'dependencies' : [
                        systemd_resolved_dependencies,
                ],
                'include_directories' : resolve_includes,
        },
        test_template + {
                'sources' : [
                        files('test-resolved-stream.c'),
                        basic_dns_sources,
                        systemd_resolved_sources,
                ],
                'dependencies' : systemd_resolved_dependencies,
                'include_directories' : resolve_includes,
        },
        test_template + {
                'sources' : [
                        files('test-resolved-dummy-server.c'),
                        basic_dns_sources,
                        systemd_resolved_sources,
                ],
                'dependencies' : systemd_resolved_dependencies,
                'include_directories' : resolve_includes,
                'type' : 'manual',
        },
        resolve_fuzz_template + {
                'sources' : files('fuzz-dns-packet.c'),
        },
        resolve_fuzz_template + {
                'sources' : files(
                        'fuzz-etc-hosts.c',
                        'resolved-etc-hosts.c',
                ),
        },
        resolve_fuzz_template + {
                'sources' : files('fuzz-resource-record.c'),
        },
]

if conf.get('ENABLE_RESOLVE') == 1
        install_data('org.freedesktop.resolve1.conf',
                     install_dir : dbuspolicydir)
        install_data('org.freedesktop.resolve1.service',
                     install_dir : dbussystemservicedir)
        install_data('org.freedesktop.resolve1.policy',
                     install_dir : polkitpolicydir)
        install_data('resolv.conf',
                     install_dir : libexecdir)

        install_symlink('resolvconf',
                        pointing_to : sbin_to_bin + 'resolvectl',
                        install_dir : sbindir)

        # symlink for backwards compatibility after rename
        install_symlink('systemd-resolve',
                        pointing_to : 'resolvectl',
                        install_dir : bindir)
endif

custom_target(
        'resolved.conf',
        input : 'resolved.conf.in',
        output : 'resolved.conf',
        command : [jinja2_cmdline, '@INPUT@', '@OUTPUT@'],
        install : conf.get('ENABLE_RESOLVE') == 1 and install_sysconfdir_samples,
        install_dir : pkgconfigfiledir)
