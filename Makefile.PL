use ExtUtils::MakeMaker;

WriteMakefile(
    'NAME'	   => 'Text::Tmpl',
    'VERSION_FROM' => 'Tmpl.pm',
    'LIBS'         => '-Llibtmpl/ -ltmpl',
    'DEFINE'       => '',
    'OBJECT'       => 'perl_tags.o Tmpl.o',
    'OPTIMIZE'     => '-O3 -g',
    'INC'          => '-Ilibtmpl/',
    'MYEXTLIB'     => 'libtmpl/libtmpl.a',
    'depend'       => {
                        'clean:'     => 'libtmpl_clean',
                        'realclean:' => 'libtmpl_clean',
                      },
    'MAN1PODS'     => {
                       'template_syntax.pod' => './blib/man1/template_syntax.1',
                       'template_extend.pod' => './blib/man1/template_extend.1',
                      },
    'MAN3PODS'     => {
                       'libtmpl.pod'         => './blib/man3/libtmpl.3',
                       'Text::Tmpl.pod'      => './blib/man3/Text::Tmpl.3',
                      },
);

sub MY::postamble {
'
$(MYEXTLIB): libtmpl/Makefile
	cd libtmpl && $(MAKE) all

libtmpl_clean: libtmpl/Makefile
	cd libtmpl && $(MAKE) clean
';
}
