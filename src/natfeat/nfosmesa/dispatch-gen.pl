#!/usr/bin/perl
# Read original include file, and generate dispatcher

$file = @ARGV[0];
$prefix = "NFOSMESA_";

if ( ! defined(open(FILE, $file)) ) {
	warn "Couldn't open $file: $!\n";
	exit;
}

print "/* Generated by dispatch-gen.pl from $file */\n\n";

$linecount=0;
while ($ligne = <FILE>) {
	if ($ligne =~ /^GLAPI/ ) {
		while (! ($ligne =~ /\);/)) {
			chomp($ligne);
			$ligne .= " " . <FILE>;
		}
		$ligne =~ s/\t//g;
		$ligne =~ s/\n//g;
		$ligne =~ s/ +/ /g;

#		print "/* $ligne */\n";

		if ($ligne =~ /^GLAPI *(\w+).* (GL)*APIENTRY *(\w+) *\(.*/) {
			$return_type = $1 ;
			$function_name = $3 ;
		} else {
			$return_type = "" ;
			$function_name = "" ;
		}

		# Remove return type, replace by "ret=" if any
		if ($ligne !~ /^GLAPI +void +(GL)*APIENTRY/ ) {
			$ligne = "ret =" . $ligne;
		}		
		$ligne =~ s/GLAPI.*(GL)*APIENTRY// ;

#print "/* $ligne */\n";

		if ( $ligne =~ /\( *void *\)/ ) {
			# Remove void list of parameters
			$ligne =~ s/\( *void *\)/\(getParameter()\)/;
		} elsif ( $ligne =~ /\( *((const)* *(\w)+) *(\w)+(\[*.*\])* *\)/ ) {
			# Replace one parameter
			$param_type = $1;
#print "/* $1 */\n";
			if ( $param_type =~ /GLdouble/ ) {
				$ligne =~ s/\( *((const)* *(\w)+) *(\w)+(\[*.*\])* *\)/(getParameter(),Atari2HostDouble(getParameter(),getParameter()))/;
			} elsif ( $param_type =~ /GLclampd/ ) {
				$ligne =~ s/\( *((const)* *(\w)+) *(\w)+(\[*.*\])* *\)/(getParameter(),(GLclampd)Atari2HostDouble(getParameter(),getParameter()))/;
			} elsif ( $param_type =~ /GLfloat/ ) {
				$ligne =~ s/\( *((const)* *(\w)+) *(\w)+(\[*.*\])* *\)/(getParameter(),Atari2HostFloat(getParameter(),getParameter()))/;
			} elsif ( $param_type =~ /GLclampf/ ) {
				$ligne =~ s/\( *((const)* *(\w)+) *(\w)+(\[*.*\])* *\)/(getParameter(),(GLclampf)Atari2HostFloat(getParameter(),getParameter()))/;
			} else {
				$ligne =~ s/\( *((const)* *(\w)+) *(\w)+(\[*.*\])* *\)/(getParameter(),getParameter())/;
			}
		} elsif ( $ligne =~ /\( *(const)* *(\w)+ *\* *((\w)+)* *\)/ ) {
			# Replace one parameter, a pointer
			$ligne =~ s/\( *((const)* *(\w)+ *\*) *((\w)+)* *\)/(getParameter(),($1)Atari2HostAddr(getParameter()))/;
		} else {
			# Add parameter for context
			$ligne =~ s/\(/(getParameter(),/;
			# Replace parameters by getParameter(n)
			if ( $ligne =~ /\( *((const)* *(\w)+ *\** *(const)*) *\** *((\w)+)* *,/) {
# print "/* $1 */\n";
				$param_type = $1;
				if ( $ligne =~ /\( *((const)* *(\w)+ *\*+ *(const)* *\**) *((\w)+)* *,/ ) {
					$ligne =~ s/\( *((const)* *(\w)+ *\*+ *(const)* *\**) *((\w)+)* *,/(($1)Atari2HostAddr(getParameter()),/;
				} else {
					if ($param_type =~ /GLdouble/ ) {
						$ligne =~ s/\( *(const)* *(\w)+ *\** *(const)* *((\w)+)* *,/(Atari2HostDouble(getParameter(),getParameter()),/;
					} elsif ($param_type =~ /GLclampd/ ) {
						$ligne =~ s/\( *(const)* *(\w)+ *\** *(const)* *((\w)+)* *,/((GLclampd)Atari2HostDouble(getParameter(),getParameter()),/;
					} elsif ($param_type =~ /GLfloat/ ) {
						$ligne =~ s/\( *(const)* *(\w)+ *\** *(const)* *((\w)+)* *,/(Atari2HostFloat(getParameter(),getParameter()),/;
					} elsif ($param_type =~ /GLclampf/ ) {
						$ligne =~ s/\( *(const)* *(\w)+ *\** *(const)* *((\w)+)* *,/((GLclampf)Atari2HostFloat(getParameter(),getParameter()),/;
					} else {
						$ligne =~ s/\( *(const)* *(\w)+ *\** *(const)* *((\w)+)* *,/(getParameter(),/;
					}
				}
			}
			while ( $ligne =~ /, *((const)* *(\w)+ *\** *(const)*) *\** *((\w)+)* *,/ ) {
# print "/* $1 */\n";
				$param_type = $1;
				if ( $ligne =~ /, *((const)* *(\w)+ *\*+ *(const)* *\**) *((\w)+)* *,/ ) {
					$ligne =~ s/, *((const)* *(\w)+ *\*+ *(const)* *\**) *((\w)+)* *,/,($1)Atari2HostAddr(getParameter()),/;
				} else {
					if ($param_type =~ /GLdouble/ ) {
						$ligne =~ s/, *(const)* *(\w)+ *\** *(const)* *\**((\w)+)* *,/,Atari2HostDouble(getParameter(),getParameter()),/;
					} elsif ($param_type =~ /GLclampd/ ) {
						$ligne =~ s/, *(const)* *(\w)+ *\** *(const)* *\**((\w)+)* *,/,(GLclampd)Atari2HostDouble(getParameter(),getParameter()),/;
					} elsif ($param_type =~ /GLfloat/ ) {
						$ligne =~ s/, *(const)* *(\w)+ *\** *(const)* *\**((\w)+)* *,/,Atari2HostFloat(getParameter(),getParameter()),/;
					} elsif ($param_type =~ /GLclampf/ ) {
						$ligne =~ s/, *(const)* *(\w)+ *\** *(const)* *\**((\w)+)* *,/,(GLclampf)Atari2HostFloat(getParameter(),getParameter()),/;
					} else {
						$ligne =~ s/, *(const)* *(\w)+ *\** *(const)* *\**((\w)+)* *,/,getParameter(),/;
					}
				}
			}
			if ( $ligne =~ /, *((const)* *(\w)+ *\** *(const)*) *\** *((\w)+)* *\)/ ) {
# print "/* $1 */\n";
				$param_type = $1;
				if ( $ligne =~ /, *((const)* *(\w)+ *\*+ *(const)* *\**) *((\w)+)* *\)/ ) {
					$ligne =~ s/, *((const)* *(\w)+ *\*+ *(const)* *\**) *((\w)+)* *\)/,($1)Atari2HostAddr(getParameter()))/;
				} else {
					if ($param_type =~ /GLdouble/ ) {
						$ligne =~ s/, *(const)* *(\w)+ *\** *(const)* *\**((\w)+)* *\)/,Atari2HostDouble(getParameter(),getParameter()))/;
					} elsif ($param_type =~ /GLclampd/ ) {
						$ligne =~ s/, *(const)* *(\w)+ *\** *(const)* *\**((\w)+)* *\)/,(GLclampd)Atari2HostDouble(getParameter(),getParameter()))/;
					} elsif ($param_type =~ /GLfloat/ ) {
						$ligne =~ s/, *(const)* *(\w)+ *\** *(const)* *\**((\w)+)* *\)/,Atari2HostFloat(getParameter(),getParameter()))/;
					} elsif ($param_type =~ /GLclampf/ ) {
						$ligne =~ s/, *(const)* *(\w)+ *\** *(const)* *\**((\w)+)* *\)/,(GLclampf)Atari2HostFloat(getParameter(),getParameter()))/;
					} else {
						$ligne =~ s/, *(const)* *(\w)+ *\** *(const)* *\**((\w)+)* *\)/,getParameter())/;
					}
				}
			}
		}

		# Add parameter number for context
		$param=0;
		while ( $ligne =~ /getParameter\(\)/ ) {
			$ligne =~ s/getParameter\(\)/getParameter($param)/;
			$param++;
		}

		# Put function name in uppercase
		$function_name =~ tr/a-z/A-Z/;
		$ligne =~ s/^ +//;
		chomp($ligne);

		print "\t\tcase $prefix$function_name:\n";
		print "\t\t\t$ligne\n";
		print "\t\t\tbreak;\n";

		$linecount++;
	}
}
close(FILE);
print "/* Functions generated: $linecount */\n";
