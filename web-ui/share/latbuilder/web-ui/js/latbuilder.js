// for IE7 --------------------------------------------------------------------

if (typeof Object.create != 'function') {
    Object.create = (function() {
        var Object = function() {};
        return function (prototype) {
            Object.prototype = prototype;
            var result = new Object();
            Object.prototype = null;
            return result;
        };
    })();
}
if (!String.prototype.trim) {
    String.prototype.trim = function () {
        // Make sure we trim BOM and NBSP
        rtrim = /^[\s\uFEFF\xA0]+|[\s\uFEFF\xA0]+$/g;
        return this.replace(rtrim, "");
    }
}

// Production steps of ECMA-262, Edition 5, 15.4.4.19
// Reference: http://es5.github.io/#x15.4.4.19
if (!Array.prototype.map) {

    Array.prototype.map = function(callback, thisArg) {

	var T, A, k;

	if (this == null) {
	    throw new TypeError(' this is null or not defined');
	}

	// 1. Let O be the result of calling ToObject passing the |this| 
	//    value as the argument.
	var O = Object(this);

	// 2. Let lenValue be the result of calling the Get internal 
	//    method of O with the argument "length".
	// 3. Let len be ToUint32(lenValue).
	var len = O.length >>> 0;

	// 4. If IsCallable(callback) is false, throw a TypeError exception.
	// See: http://es5.github.com/#x9.11
	if (typeof callback !== 'function') {
	    throw new TypeError(callback + ' is not a function');
	}

	// 5. If thisArg was supplied, let T be thisArg; else let T be undefined.
	if (arguments.length > 1) {
	    T = thisArg;
	}

	// 6. Let A be a new array created as if by the expression new Array(len) 
	//    where Array is the standard built-in constructor with that name and 
	//    len is the value of len.
	A = new Array(len);

	// 7. Let k be 0
	k = 0;

	// 8. Repeat, while k < len
	while (k < len) {

	    var kValue, mappedValue;

	    // a. Let Pk be ToString(k).
	    //   This is implicit for LHS operands of the in operator
	    // b. Let kPresent be the result of calling the HasProperty internal 
	    //    method of O with argument Pk.
	    //   This step can be combined with c
	    // c. If kPresent is true, then
	    if (k in O) {

		// i. Let kValue be the result of calling the Get internal 
		//    method of O with argument Pk.
		kValue = O[k];

		// ii. Let mappedValue be the result of calling the Call internal 
		//     method of callback with T as the this value and argument 
		//     list containing kValue, k, and O.
		mappedValue = callback.call(T, kValue, k, O);

		// iii. Call the DefineOwnProperty internal method of A with arguments
		// Pk, Property Descriptor
		// { Value: mappedValue,
		//   Writable: true,
		//   Enumerable: true,
		//   Configurable: true },
		// and false.

		// In browsers that support Object.defineProperty, use the following:
		// Object.defineProperty(A, k, {
		//   value: mappedValue,
		//   writable: true,
		//   enumerable: true,
		//   configurable: true
		// });

		// For best browser support, use the following:
		A[k] = mappedValue;
	    }
	    // d. Increase k by 1.
	    k++;
	}

	// 9. return A
	return A;
    };
}

//-----------------------------------------------------------------------------

var DEFAULT_WEIGHT = 0.1;

var PAT_INTEGER  = /^[1-9]+[0-9]*$/;
var PAT_NORM     = /^(\d*\.?\d+|\d+\.?\d*)(e[\+-]?[0-9]+)?$|^inf$/;
var PAT_REAL_POS = /^(\d*\.?\d+|\d+\.?\d*)(e[\+-]?[0-9]+)?$/;
var PAT_REAL     = /^[\+-]?(\d*\.?\d+|\d+\.?\d*)(e[\+-]?[0-9]+)?$/;

var FIGURES = {
     '{CU}P{alpha}': {
	name:  "P<sub>α</sub>",
	alpha: "\\(\\alpha = 2,4,6\\)"
    },
     '{CU}R{alpha}': {
	name:  "R<sub>α</sub>",
	alpha: "\\(\\alpha > 0\\)"
    },
     'spectral': {
	name:  "spectral"
    }
};

var CONSTRUCTIONS = {
    'explicit:{gen}': {
	name: "Explicit (Evaluation)",
	desc: "<h5>Evaluation</h5><p>Evaluates the figure of merit for a given generating vector \\(\\boldsymbol a = (a_1, \\ldots, a_s)\\).</p>"
    },
    'exhaustive': {
	name: "Exhaustive",
	desc: "<h5>Exhaustive Search</h5><p>Examines all generating vectors and retains the best one.</p>"
    },
    'random:{nrand}': {
	name: "Random",
	desc: "<h5>Random Search</h5><p>Examines a number \\(r\\) of randomly selected generating vectors \\(\\boldsymbol a\\) and retains the best one.</p>",
	rand: "Number \\(r\\) of Random Samples of \\(\\boldsymbol a\\)"
    },
    'Korobov': {
	name: "Korobov",
	desc: "<h5>Korobov Search</h5><p>Examines all generating vectors of the form \\(\\boldsymbol a = (1, a \\bmod n, a^2 \\bmod n, …, a^s \\bmod n)\\) and retains the best one.</p>"
    },
    'random-Korobov:{nrand}': {
	name: "Random Korobov",
	desc: "<h5>Random Korobov Search</h5><p>Examines a number \\(r\\) of randomly selected generating vectors of the form \\(\\boldsymbol a = (1, a \\bmod n, a^2 \\bmod n, …, a^s \\bmod n)\\) and retains the best one.</p>",
	rand: "Number \\(r\\) of Random Samples of \\(a\\)"
    },
    'CBC': {
	name: "CBC",
	desc: "<h5>Component-by-Component Construction</h5><p>Examines all possible values of the components \\(a_j\\) of the generating vector \\(\\boldsymbol a = (a_1, \\ldots, a_s)\\) and selects the best ones, one coordinate at a time.</p>"
    },
    'random-CBC:{nrand}': {
	name: "Random CBC",
	desc: "<h5>Random Component-by-Component Construction</h5><p>Examines a number \\(r\\) of randomly selected values for each component \\(a_j\\) of the generating vector \\(\\boldsymbol a = (a_1, \\ldots, a_s)\\) and selects the best ones, one coordinate at a time.</p>",
	rand: "Number \\(r\\) of Random Samples of \\(a_j\\) for each \\(j\\)"
    },
    'fast-CBC': {
	name: "Fast CBC",
	desc: "<h5>Fast Component-by-Component Construction</h5><p>Examines all possible values of the components components \\(a_j\\) of the generating vector \\(\\boldsymbol a = (a_1, \\ldots, a_s)\\) and selects the best ones, one coordinate at a time.  Computation is accelerated by using fast Fourier transforms.</p><p>Requires the Coordinate-Uniform evaluation method.</p>",
	needsCU: true
    }
};
            
//-----------------------------------------------------------------------------

function LatSize(str) {
    if (str.match(/\^/)) {
        var a = str.split(/\^/);
        this.base = parseInt(a[0]);
        this.exp  = parseInt(a[1]);
        this.size = Math.pow(this.base, this.exp);
    }
    else {
        this.size = parseInt(str);
    }
}
LatSize.prototype = {
    value: function() {
        return this.size;
    },
    toString: function() {
        if (this.base) {
            return this.base + '^' + this.exp;
        }
        else {
            return '' + this.size;
        }
    }
}

function formatTime(seconds) {
    var s = seconds % 60
        var m = (seconds - s) / 60
        s = Math.round(s * 1000) / 1000;
    if (m == 0) {
        return s + " s";
    }
    else {
        return m + " min " + s + " s";
    }
}


(function($) {
    $.fn.extend({

	// ===========================================================================
	// MathJax
	// ===========================================================================

	typeset: function() {
	    var self = $(this);
	    MathJax.Hub.Queue(['Typeset', MathJax.Hub, self.get()]);
	    return self;
	},

	// ===========================================================================
	// validation
	// ===========================================================================

	validate: function(pat) {
	    var self = $(this);
	    if (self.prop('tagName').toLowerCase() != 'input') {
		return self.find('input').validate(pat);
	    }
	    var val = self.val();
	    var val = self.val().trim();
	    var re = new RegExp(pat);
	    var err = $("#" + self.attr('id') + "-errmsg");
	    if (re.test(val) || val == "") {
		self.removeClass("invalid-text");
		if (err) { err.slideUp("fast"); }
		return true;
	    }
	    else {
		self.addClass("invalid-text");
		if (err) { err.slideDown("fast"); }
		return false;
	    }
	},

	// ===========================================================================
	// input array
	// ===========================================================================

	// attr: {caption, index, pattern, size, defVal}
	inputArray: function(attr) {
	    var self = $(this);
	    self.addClass('inputArray');
	    self.data('pattern', attr.pattern);
	    self.data('defVal', attr.defVal);
	    var list = $('<ul>').append($('<li>')
		    .append($('<div class="caption">').html(attr.caption !== undefined ? attr.caption + '&thinsp;:' : ''))
		    .append($('<div class="caption index">').html(attr.index !== undefined ? attr.index + '&thinsp;:' : '')));
	    if (attr.size !== undefined) {
		self.inputArraySize(attr.size);
	    }
	    self.html(list).addClass();
	    list.typeset();
	    return self;
	},

	inputArrayValues: function(vals) {
	    var self = $(this);

	    if (!self.hasClass('inputArray')) {
	        return self.find('.inputArray').each(function(){ $(this).inputArrayValues(vals) });
	    }

	    if (vals === undefined) {
		// return values
		var vals = self.find('input').map(function() { return this.value; });
		if (vals) {
		    return vals.get();
		}
		else {
		    return [];
		}
	    }
	    else {
		// set values
		self.inputArraySize(vals.length);
		self.find('input').each(function(i) {
		    $(this).val(vals[i]);
		});
		return self;
	    }
	},

	inputArraySize: function(size) {
	    var self = $(this);

	    if (!self.hasClass('inputArray')) {
	        return self.find('.inputArray').each(function(){ $(this).inputArraySize(size) });
	    }

	    // return size
	    if (size === undefined) {
		return self.find('input').size();
	    }
	    // change size
	    else {
		var oldSize = self.inputArraySize();
		// do nothing
		// remove last elements
		if (size < oldSize) {
		    self.find('input').each(function(i) {
			if (i >= size) {
			    $(this).parent().remove();
			}
		    });
		}
		// append elements
		else if (size > oldSize) {
		    var newVal = self.find('input').last().val();
		    if (newVal === undefined || newVal.trim() == '') {
			newVal = self.data('defVal');
		    }
		    var list = self.find('ul').first();
		    var pat = self.data('pattern');
		    for (var i = oldSize; i < size; i++) {
			var input = $('<input type="text">').val(newVal);
			// validation
			if (pat) {
			    input.on("keyup", (function(pat) { return function() { $(this).validate(pat); } })(pat));
			}
			list.append($('<li>')
				.append(input)
				.append($('<div class="index">').html(i + 1))
				);
		    }
		    catchEnter();
		}
		return self;
	    }
	},

	// ===========================================================================
	// weights
	// ===========================================================================

	weights: function(attr) {
	    var self = $(this);
	    self.addClass("weights-group");
	    var math = $('<div>').html(attr.display).typeset();
	    self.append($('<div class="three columns alpha">')
		    .append($('<div class="label">').html(attr.title))
		    .append(math)
		    .append($('<a>')
			.attr('href', '#')
			.text('[remove]')
			.on('click', function(e) {
			    e.preventDefault();
			    self.slideUp("fast", function() { self.remove(); });
			}))
		    );
	    // custom contents
	    if (attr.custom !== undefined) {
		self.append(attr.custom);
	    }
	    // inputArray contents
	    else {
		var array_container = $('<div class="thirteen columns omega clearfix">');
		$.each(attr.arrays, function(i) {
		    var array = $('<div>').inputArray(attr.arrays[i]);
		    var label = $('<div class="label">')
			.html(attr.arrays[i].name)
			// [expression] link
			.append($('<a>')
				.attr('href', '#')
				.append('[expression (set all weights at once)]')
				.on('click', makeExpressionDialog(array, attr.arrays[i])));
		    array_container.append(label);
		    array_container.append(array);
		});
		self.append(array_container);
	    }
	    return self;
	},

	asArg: function(f) {
	    var self = $(this);
	    // call function
	    if (f === undefined) {
		var f = self.data('asArg');
		if (f === undefined) {
		    return f.val();
		}
		return f.call(this);
	    }
	    else {
		self.data('asArg', f);
		return self;
	    }
	}
    });
})(jQuery);


function addWeights(wtype) {

    var COORD_WEIGHTS = {
        name:      'Coordinate Weights',
        pattern:   PAT_REAL,
        caption:   '\\(\\gamma_j^p\\)',
        index:     'j',
        indexText: 'coordinate index',
        defVal:    DEFAULT_WEIGHT};
    var ORDER_WEIGHTS = {
        name:      'Order Weights',
        pattern:   PAT_REAL,
        caption:   '\\(\\Gamma_k^p\\)',
        index:     'k',
        indexText: 'projection order',
        defVal:    DEFAULT_WEIGHT};
    var WEIGHTS = {
        'product': {
            title:   'Product Weights',
            display: '\\[ \\gamma_u = \\prod_{j \\in u} \\gamma_j \\]',
            arrays:  [COORD_WEIGHTS],
            asArg:   function() {
                var v0 = $(this).find('.inputArray').inputArrayValues();
                return 'product:' + v0[v0.length - 1] + ':' + v0.join(',');
            }
        },
        'order-dependent': {
            title:   'Order-Dependent Weights',
            display: '\\[ \\gamma_u = \\Gamma_{|u|} \\]',
            arrays:  [ORDER_WEIGHTS],
            asArg:   function() {
                var v0 = $(this).find('.inputArray').inputArrayValues();
                return 'order-dependent:' + v0[v0.length - 1] + ':' + v0.join(',');
            }
        },
        'POD': {
            title:   'POD Weights',
            display: '\\[ \\gamma_u = \\Gamma_{|u|} \\prod_{j \\in u} \\gamma_j \\]',
            arrays:  [ORDER_WEIGHTS, COORD_WEIGHTS],
            asArg:   function() {
                var v = $(this).find('.inputArray');
                var v0 = v.eq(0).inputArrayValues();
                var v1 = v.eq(1).inputArrayValues();
                return 'POD:'  + v0[v0.length - 1] + ':' + v0.join(',')
                    + ':' + v1[v1.length - 1] + ':' + v1.join(',');
            }
        },
        'projection-dependent': {
            title:   'Projection-Dependent Weights',
            display: '',
            custom:  $('<div class="thirteen columns omega clearfix">')
                .append($('<div class="four columns alpha">')
                        .html($('<textarea class="three columns" style="height: 160px">')))
                .append($('<div class="four columns">')
                        .html("Enter the mapping between coordinates and weights.  " +
                            "Each line must be <b>comma-separated list of coordinates</b> " +
                            "followed by a <b>colon</b> and a <b>weight value</b>.  " +
                            "Spaces are ignored.<br/>" +
                            "Example line: <code>1,2,5: 0.7</code>.")),
            asArg:   function() {
                var ret = 'projection-dependent';
                var lines = $(this).find('textarea').val().split('\n');
                lines = lines.map(function(s){ return s.trim().replace(/\s/g, ''); });
		$.each(lines, function(i) {
                    var l = lines[i];
                    if (l.length > 0) {
                        ret = ret + ':' + l;
                    }
                });
                return ret;
            }
        }
    };

    var w = $('<div class="sixteen columns">').weights(WEIGHTS[wtype]).hide().asArg(WEIGHTS[wtype].asArg);
    w.inputArraySize($('#dimension').val());
    $('#weights-panel').append(w);
    w.typeset().slideDown();
}

function getBackend() {
    $.ajax({
        url: "services/lbjson.py",
        type: "post",
        data: JSON.stringify({'id': nextRequestId(), 'method': 'backend_version', 'params':{}}),
        dataType: "json",
        error: function(response) {
            $('#backend-version')
                .html("<u>ERROR</u><br/>Cannot communicate with the Lattice Builder CGI service. "
                        + "The search function will not work.")
                .addClass('errmsg');
        },
        success: function(response) {
            if (response.error) {
                $('#backend-version')
                    .html("<u>ERROR</u><br/>" + response.error.message + "<br/>"
                        + "The search function will not work.")
                    .addClass('errmsg');
            }
            else {
                $('#backend-version')
                    .html("<strong>Backend:</strong> " + response.result)
                    .removeClass('errmsg');
            }
        }
    });
}

function arrayFromExpr(expr, array, attr) {
    $.ajax({
        url: "services/lbjson.py",
        type: "post",
        data: JSON.stringify({'id': nextRequestId(), 'method': 'array_from_expr', 'params':
	    [expr, attr.index, array.inputArraySize()]}),
        dataType: "json",
        error: function(response) {
            alert("ERROR: Cannot communicate with the Lattice Builder CGI service.");
        },
        success: function(response) {
            if (response.error) {
                alert("ERROR: " + response.error.message);
            }
            else {
		var a = response.result;
		if (attr.transform) {
		    a = a.map(attr.transform);
		}
                array.inputArrayValues(a);
		if (attr.pattern) {
		    array.validate(attr.pattern);
		}
            }
        }
    });
}

function catchEnter() {
    // prevent sending on ENTER
    $('#criteria-form input, #criteria-form select').on('keypress', function(event) {
	if (event.keyCode == 13) {
	    event.preventDefault();
	    $(event.target).blur();
	}
    });
}

function executeLatBuilder() {
    var lat   = $('#embedded').prop('checked') && 'embedded' || 'ordinary';
    var size  = $('#size').val();
    var dim   = $('#dimension').val();
    var norm  = $('#norm-type').val();
    var alpha = $('#figure-alpha').val();
    var CU    = $('#coord-uniform').prop('checked') && norm == 2 ? 'CU:' : '';
    var fig   = $('#figure').val().replace(/{alpha}/, alpha).replace(/{CU}/, CU);
    var wpow = $('#weights-power').val();
    var weights = [];
    $('.weights-group').each(function() {
        weights.push($(this).asArg());
    });
    var cons = $('#construction').val()
	.replace(/{nrand}/, $('#construction-nrand').val())
	.replace(/{gen}/, $('#construction-gen').inputArrayValues().join(','));

    var mlFilters = [];
    var combiner = '';
    if (lat == 'embedded') {
	if ($('#normalization-active').is(':checked')) {
	    var ntype = $('#normalization').val().replace(/{alpha}/, alpha);
	    mlFilters.push('norm:'+ ntype + ':even:' + $('#level-min').val() + ',' + $('#level-max').val());
	}
	if ($('#low-pass-active').is(':checked')) {
	    mlFilters.push('low-pass:' + $('#low-pass-threshold').val());
	}
	combiner = $('#combiner').val();
    }

    $('#search-button').hide();
    $('#abort-button').show();
    params = [lat, size, dim, norm, fig, cons, weights, wpow, '', mlFilters, combiner];
    
    var xhr = $.ajax({
        url: "services/lbjson.py",
        type: "post",
        data: JSON.stringify({'id': nextRequestId(), 'method': 'latbuilder_exec', 'params': params}),
        dataType: "json",
        error: function(response) {
            $('#status')
                .html("<u>ERROR</u><br/>Cannot communicate with the Lattice Builder CGI service.")
                .addClass('errmsg');
		$('#results').slideUp();
		$('#search-button').show();
		$('#abort-button').hide();
		$('.with-results').hide();
		$('.without-results').show();
        },
        success: function(response) {
            if (response.error) {
                $('#status')
                    .html("<u>ERROR</u><br/>" + response.error.message)
                    .addClass('errmsg');
		$('#results').slideUp();
		$('#search-button').show();
		$('#abort-button').hide();
		$('.with-results').hide();
		$('.without-results').show();
            }
            else {
                $('#status')
                    .html("Search successful.")
                    .removeClass('errmsg');
		$("#results-command").text(response.result[0]);
		$("#results-size").text(response.result[1]);
		$("#results-gen").text(response.result[2].join(', '));
		$("#results-merit").text(response.result[3]);
		$("#results-cpu-time").text(formatTime(response.result[4]));
		$('#results').slideDown();
		$('#search-button').show();
		$('#abort-button').hide();
		$('.with-results').show();
		$('.without-results').hide();
		$('html, body').animate({scrollTop: $("#results").offset().top}, 600);
            }
        }
    });

    $('#abort-button').on('click', function(e) {
	e.preventDefault();
	xhr.abort();
	$('#search-button').show();
	$('#abort-button').hide();
	$('#status')
	    .html("Search aborted.")
	    .addClass('errmsg');
    });
}

function adjustFigureAlpha() {
    if ($('#figure').val().indexOf('P{alpha}') >= 0) {
	var x = 2 * Math.max(1, Math.min(3, Math.round($('#figure-alpha').val() / 2)));
	$('#figure-alpha').val(x);
    }
}

function makeExpressionDialog(array, attr) { return function(e) {
    e.preventDefault();
    var dialog = $('#expression-dialog');
    var input = dialog.find('input');
    var form = dialog.find('form');
    dialog.find('.expr-val').html(attr.caption);
    dialog.find('.expr-name').html(attr.name.toLowerCase());
    dialog.find('.expr-var').html(attr.index);
    dialog.find('.expr-desc').html(attr.indexText);
    form.off('submit');
    form.on('submit', function(e) {
	e.preventDefault();
	var val = input.val();
	arrayFromExpr(val, array, attr);
	dialog.jqmHide();
    });
    dialog.typeset().jqmShow();
    input.focus().select();
}}

function makeLattice(n, gen) {
    var points = [];
    for (var i = 0; i < n; i++) {
	p = [];
	for (var j in gen) {
	    p.push((i * gen[j] % n) / n);
	}
	points.push(p);
    }
    return points;
}

$('document').ready(function() {

    $('#size').on('change', function() {
	var size = new LatSize($(this).val());
	if (size && size.exp) {
	    $('#level-max').val(size.exp);
	    $('#level-max').trigger('change');
	}
    });
    $('#embedded').on('click', function() {
	if ($(this).is(':checked')) {
	    $('#multilevel-panel').slideDown();
	}
	else {
	    $('#multilevel-panel').slideUp();
	}
    });
    $('#dimension').on('change', function() {
	var dim = $(this).val();
	$('.weights-group').inputArraySize(dim);
	$('#construction-gen').inputArraySize(dim);
    });
    $('#normalization-active').on('click', function() {
	if ($(this).is(':checked')) {
	    $('#normalization-panel').slideDown();
	}
	else {
	    $('#normalization-panel').slideUp();
	}
    });
    $('#low-pass-active').on('click', function() {
	if ($(this).is(':checked')) {
	    $('#low-pass-panel').slideDown();
	}
	else {
	    $('#low-pass-panel').slideUp();
	}
    });
    $('#level-min').on('change', function() {
	var val = parseInt($(this).val());
	// check against lattice size
	var size = new LatSize($('#size').val());
	if (size && size.exp && val > size.exp) {
	    val = size.exp;
	    $(this).val(val);
	}
	// update max level if needed
	if ($('#level-max').val() < val) {
	    $('#level-max').val(val);
	}
    });
    $('#level-max').on('change', function() {
	var val = parseInt($(this).val());
	// check against lattice size
	var size = new LatSize($('#size').val());
	if (size && size.exp && val > size.exp) {
	    val = size.exp;
	    $(this).val(val);
	}
	// update min level if needed
	if ($('#level-min').val() > val) {
	    $('#level-min').val(val);
	}
    });
    $('#norm-type').on('change', function() {
	var norm = $(this).val();
	$('#weights-power').val(norm == "inf" ? 1.0 : norm);
	if (norm != 2) {
	    // should not happen!
	    $('#coord-uniform').removeAttr('checked');
	    $('#coord-uniform').triggerHandler('click');
	}
    });
    $('#norm-type-2').on('click', function() {
	var div = $('#coord-uniform').parent();
	div.css('transition-duration', '800ms');
	div.addClass('alert');
	window.setTimeout(function() { div.removeClass('alert'); }, 800);
    });
    $('#coord-uniform').on('click', function() {
	var checked = $(this).is(':checked');
	var pout, pin;
	if (checked) {
	    $('#norm-type').val(2).trigger('change');
	    pout = $('#norm-type-panel');
	    pin  = $('#norm-type-2');
	}
	else {
	    pout = $('#norm-type-2');
	    pin  = $('#norm-type-panel');
	}
	var cons = $('#construction');
	cons.find('option').each(function() {
	    var self = $(this);
	    if (CONSTRUCTIONS[self.val()].needsCU && !checked) {
		if (cons.val() == self.val()) {
		    cons.val('CBC');
		    cons.triggerHandler('change');
		}
		self.attr('disabled', 'disabled');
	    }
	    else {
		self.removeAttr('disabled');
	    }
	});
	pin.clearQueue().finish();
	pout.clearQueue().finish().fadeOut('fast', function() { pin.fadeIn('fast'); });
    });
    $('#figure').on('keyup change', function() {
	var fig = $(this).val();
	if (fig.indexOf('{CU}') >= 0) {
	    $('#coord-uniform').removeAttr('disabled');
	    $('#coord-uniform').triggerHandler('click');
	}
	else {
	    $('#coord-uniform').removeAttr('checked').attr('disabled', 'disabled');
	    $('#coord-uniform').triggerHandler('click');
	}
	if (fig.indexOf('{alpha}') >= 0) {
	    $('#figure-alpha-var').html(FIGURES[fig].alpha);
	    $('#figure-alpha').parent().finish().fadeIn().typeset();
	}
	else {
	    $('#figure-alpha').parent().finish().fadeOut();
	}
	adjustFigureAlpha();
    });
    $('#weights-power').on('change', function() {
	var self = $(this);
	if (self.val().trim().toLowerCase() == 'q') {
	    self.val($('#norm-type').val());
	}
    });
    $('#construction').on('keyup change', function() {
	var cons = $(this).val();
	$('#construction-desc').html(CONSTRUCTIONS[cons].desc).typeset();
	if (cons.indexOf('{nrand}') >= 0) {
	    $('#construction-nrand-label').html(CONSTRUCTIONS[cons].rand).typeset();
	    $('#construction-nrand').parent().slideDown();
	}
	else {
	    $('#construction-nrand').parent().slideUp();
	}
	if (cons.indexOf('{gen}') >= 0) {
	    $('#construction-gen').parent().slideDown();
	}
	else {
	    $('#construction-gen').parent().slideUp();
	}
    });
    var GEN_ATTR = {
	caption:   '\\(a_j\\)',
	name:      'components of the generating vector (modulo \\(n\\))',
	index:     'j',
	indexText: 'coordinate index',
	pattern:   PAT_INTEGER,
	defVal:    1,
	transform: function(x) { return x % (new LatSize($('#size').val())).size; }
    };
    $('#construction-gen').inputArray(GEN_ATTR);
    $('#construction-gen-panel .expression-dialog-trigger')
	.on('click', makeExpressionDialog($('#construction-gen'), GEN_ATTR));
    $('#construction-gen-panel .from-results-trigger').on('click', function(e) {
	e.preventDefault();
	var self = $(this);
	var res = $('#results-gen').text().split(',').map(function(x) { return x.trim(); });
	var gen = $('#construction-gen').inputArrayValues();
	for (var i = 0; i < Math.min(gen.length, res.length); i++) {
	    gen[i] = res[i];
	}
	$('#construction-gen').inputArrayValues(gen);
    });

    // validation
    $('#size').on('keyup change', function() {
	if ($(this).validate(/^[1-9][0-9]*(\^[1-9][0-9]*)?$/)) {
	    var size = new LatSize($(this).val());
	    if (size && size.base) {
		$('#size-str').text(' = ' + size.value()).show();
		return;
	    }
	}
	$('#size-str').text("").hide();
    });
    $('#dimension').on('keyup change', function() { $(this).validate(PAT_INTEGER); });
    $('#level-min').on('keyup change', function() { $(this).validate(PAT_INTEGER); });
    $('#level-max').on('keyup change', function() { $(this).validate(PAT_INTEGER); });
    $('#low-pass-threshold').on('keyup change', function() { $(this).validate(PAT_REAL); });
    $('#figure-alpha').on('keyup', function() { $(this).validate(PAT_REAL_POS); });
    $('#figure-alpha').on('change', adjustFigureAlpha);
    $('#norm-type').on('keyup change', function() { $(this).validate(PAT_NORM); });
    $('#weights-power').on('keyup change', function() { $(this).validate(PAT_REAL_POS); });
    $('#construction-nrand').on('keyup change', function() { $(this).validate(PAT_INTEGER); });
    $('#results-command-toggle').on('click', function(e) {
	e.preventDefault();
	$('#results-command').slideToggle();
    });
    $('#plot-coordinate1').add('#plot-coordinate2').on('keyup change', function() { $(this).validate(PAT_INTEGER); });

    // dialogs
    $('#expression-dialog').jqm({trigger: false});
    $('#new-weights-dialog').jqm({trigger: 'a.new-weights-dialog-trigger'});
    $('#new-weights-dialog a').on('click', function(e) {
	e.preventDefault();
	var wtype = e.target.id.substr(12);
	$('#new-weights-dialog').jqmHide();
	addWeights(wtype);
    });
    // code
    $('#code-dialog').jqm({trigger: 'a.code-dialog-trigger'});
    $('#code-c').on('click', function(e) {
	var gen = $('#results-gen').text().split(',').map(function(x) { return x.trim(); });
	$('#code-dialog h4').html('C Code');
	$('#code-dialog textarea').html(
		'int n = ' + $('#results-size').text() + ';\n' +
		'int s = ' + gen.length + ';\n' +
		'int a[] = {' + $('#results-gen').text()  + '};\n' +
		'double points[n][s];\n' +
		'int i, j;\n' +
		'for (i = 0; i < n; i++)\n' +
		'  for (j = 0; j < s; j++)\n' +
		'    points[i][j] = ((long long)i * a[j]) % n / (double)n;')
	    .focus().select();
    });
    $('#code-python').on('click', function(e) {
	$('#code-dialog h4').html('Python Code');
	$('#code-dialog textarea').html(
		'n = ' + $('#results-size').text() + '\n' +
		'a = [' + $('#results-gen').text() + ']\n' +
		'points = [[(i * aj % n) / float(n) for aj in a] for i in range(n)]')
	    .focus().select();
    });
    $('#code-matlab').on('click', function(e) {
	$('#code-dialog h4').html('Matlab Code');
	$('#code-dialog textarea').html(
		'n = ' + $('#results-size').text() + ';\n' +
		'a = [' + $('#results-gen').text() + '];\n' +
		'points = zeros(n,length(a));\n' +
		'for i = 1:n\n' +
		'    points(i,:) = mod((i - 1) * a, n) / n;\n' +
		'end')
	    .focus().select();
    });
    // plot
    $('#plot-dialog').jqm({trigger: 'a.plot-dialog-trigger'});
    $('.plot-dialog-trigger').on('click', function(e) {
	$('#plot-coordinate1').val(Math.max(1,Math.min($('#plot-coordinate1').val(), $('#dimension').val())));
	$('#plot-coordinate2').val(Math.max(1,Math.min($('#plot-coordinate2').val(), $('#dimension').val())));
	$('#plot-coordinate1').triggerHandler('change');
    });
    function coordInc(c) {
	var val = parseInt(c.val()) + 1;
	if (val <= $('#dimension').val()) {
	    c.val(val).triggerHandler('change');
	}
    }
    function coordDec(c) {
	var val = parseInt(c.val()) - 1;
	if (val >= 1) {
	    c.val(val).triggerHandler('change');
	}
    }
    $('#plot-coordinate1-dec').on('click', function(e) { e.preventDefault(); coordDec($('#plot-coordinate1')); });
    $('#plot-coordinate1-inc').on('click', function(e) { e.preventDefault(); coordInc($('#plot-coordinate1')); });
    $('#plot-coordinate2-dec').on('click', function(e) { e.preventDefault(); coordDec($('#plot-coordinate2')); });
    $('#plot-coordinate2-inc').on('click', function(e) { e.preventDefault(); coordInc($('#plot-coordinate2')); });
    $('#plot-coordinate1').add('#plot-coordinate2').on('change', function() {
	var plot_options = {
	    lines:  { show: false },
	    points: { show: true, radius: 2, fill: true, fillColor: '#4C6B8B' },
	    colors: ['#4C6B8B'],
	    xaxis:  { min: 0, max: 1 },
	    yaxis:  { min: 0, max: 1 },
	};
	var n = $('#results-size').text();
	var gen = $('#results-gen').text().split(',').map(function(x) { return x.trim(); });
	var j1 = parseInt($('#plot-coordinate1').val()) - 1;
	var j2 = parseInt($('#plot-coordinate2').val()) - 1;
	$.plot($("#plot"), [ makeLattice(n, [gen[j1],gen[j2]]) ], plot_options);
    });

    // populate lists
    var list = $('#figure');
    $.each(FIGURES, function(x) {
	list.append($('<option>').val(x).html(FIGURES[x].name));
    });
    var list = $('#construction');
    $.each(CONSTRUCTIONS, function(x) {
	list.append($('<option>').val(x).html(CONSTRUCTIONS[x].name));
    });

    // initial values
    $('#size').val('2^10');
    $('#dimension').val(3);
    $('#level-min').val(1);
    $('#low-pass-threshold').val(1.0);
    $('#figure-alpha').val(2);
    $('#norm-type').val(2);
    $('#construction').val('CBC');
    $('#construction-nrand').val(30);

    // ensure consitency
    $('#size').triggerHandler('change');
    $('#embedded').triggerHandler('click');
    $('#dimension').triggerHandler('change');
    $('#normalization-active').triggerHandler('click');
    $('#low-pass-active').triggerHandler('click');
    $('#figure').triggerHandler('change');
    $('#norm-type').triggerHandler('change');
    $('#coord-uniform').triggerHandler('click');
    $('#construction').triggerHandler('change');

    addWeights('product');


    // search
    $('#criteria-form').on('submit', function(e) {
	e.preventDefault();
	if ($('.invalid-text').size() > 0) {
	    window.alert('There are invalid values in the input boxes; please check above for error messages.');
	    return;
	}
	if ($('.weights-group').size() == 0) {
	    window.alert('No weights are specified; please add at least one type of weights.');
	    return;
	}
	executeLatBuilder();
    });

    catchEnter();

    $(".errmsg").hide();
    $("#waiting").hide();
    $(document).ajaxStart(function() { $('#status').hide(); $("#waiting").fadeIn(); });
    $(document).ajaxStop(function() { $("#waiting").fadeOut(function() { $('#status').show(); }); });

    getBackend();

    $('#size').focus();
});

var _cur_request_id = 0;
function nextRequestId() {
    _cur_request_id = _cur_request_id + 1;
    return _cur_request_id;
}

