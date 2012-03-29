#!/usr/bin/perl

my $scale = 6.5;
my $offset = 360;
my $suffix = "_rad";

my @vertices;
my @faces;

while(<>) {
	if( $_ =~ /v ([^ ]+) ([^ ]+) ([^ ]+)/ ) {
		push @vertices, [$1, $2, $3];
	}

	if( $_ =~ /f ([0-9]+)\/\/[0-9]+ ([0-9]+)\/\/[0-9]+ ([0-9]+)\/\/[0-9]+/ ) {
		push @faces, [$1, $2, $3];
	}
}

print "#define numVertices$suffix " . scalar @vertices . "\n";
print "const vertex_t vertices" . $suffix . "[] = {\n";
foreach(@vertices) {
	my @vertex = @{$_};
	print "\t{ { F(" . $vertex[0]*$scale .
		 "), F(" . $vertex[1]*$scale .
		 "), F(" . $vertex[2]*$scale . "), F(1) }, ";
	print "RGB(" . int(rand(8)) . ", " . int(rand(8)) . ", " . int(rand(4)) . ") },\n";
}
print "};\n\n";

print "#define numFaces$suffix " . scalar @faces . "\n";
print "const index_triangle_t faces" . $suffix . "[] = {";
foreach(@faces) {
	my @face = @{$_};
	print "\t{" . ($face[0] - 1 + $offset) . ", " . ($face[1] - 1 + $offset) . ", " . ($face[2] - 1 + $offset) . "},\n";
}
print "};\n";
