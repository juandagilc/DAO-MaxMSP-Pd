autowatch = 1

var p = this.patcher

function build (n, signals) {
	var h = 15
	var v = 90
	
	// create source
	var old_source = p.getnamed ("source")
	p.remove (old_source)
	
	var source = p.newdefault (h+n, v, "cycle~", "250")
	source.varname = "source"
	
	// create signals
	if (signals) {
		var old_signal1 = p.getnamed ("signal1")
		p.remove (old_signal1)
		
		var signal1 = p.newdefault (h+n+110, v, "sig~", 2.0)
		signal1.varname = "signal1"
		
		var old_signal2 = p.getnamed ("signal2")
		p.remove (old_signal2)
		
		var signal2 = p.newdefault (h+n+220, v, "sig~", 0.5)
		signal2.varname = "signal2"
	}
	
	// create object
	var old_object
	var object
	for (var ii = 0; ii < n; ii++) {
		old_object = p.getnamed ("object" + ii)
		p.remove (old_object)
		
		object = p.newdefault (h+ii, v+ii+30, "vpdelay~", 10, 10, 0.5)
		object.varname = "object" + ii
		
		p.connect (source, 0, object, 0)
		
		if (signals) {
			p.connect (signal1, 0, object, 1)
			p.connect (signal2, 0, object, 2)
		}
	}
}
