#!/usr/bin/perl

my $scale = 2.0;
my $offset = 0;
my $suffix = "";

my @vertices;
my @faces;
my @normals;

while(<>) {
	if( $_ =~ /v ([^ ]+) ([^ ]+) ([^ ]+)/ ) {
		push @vertices, [$1, $2, $3];
	}

	if( $_ =~ /f ([0-9]+)\/\/[0-9]+ ([0-9]+)\/\/[0-9]+ ([0-9]+)\/\/[0-9]+/ ) {
		push @faces, [$1, $2, $3];
	}

	if( $_ =~ /vn ([^ ]+) ([^ ]+) ([^ ]+)/ ) {
		push @normals, [$1, $2, $3];
	}
}

print "#define numVertices$suffix " . scalar @vertices . "\n";
print "const init_vertex_t vertices" . $suffix . "[] = {\n";
my $idx = 0;
foreach(@vertices) {
	my @vertex = @{$_};
	my @normal = @{$normals[$idx]};
	print "\t{ { F(" . $vertex[0]*$scale .
		 "), F(" . $vertex[1]*$scale .
		 "), F(" . $vertex[2]*$scale . ") }, ";
	print "{ F(" . $normal[0]*1.0 .
	      "), F(" . $normal[1]*1.0 .
	      "), F(" . $normal[2]*1.0 . ") }";
	#print "RGB(" . int(rand(8)) . ", " . int(rand(8)) . ", " . int(rand(4)) . ")";
	print " },\n";
	$idx++;
}
print "};\n\n";

print "#define numFaces$suffix " . scalar @faces . "\n";
print "const index_triangle_t faces" . $suffix . "[] = {\n";
foreach(@faces) {
	my @face = @{$_};
	print "\t{" . ($face[0] - 1 + $offset) . ", " . ($face[1] - 1 + $offset) . ", " . ($face[2] - 1 + $offset) . "},\n";
}
print "};\n";
