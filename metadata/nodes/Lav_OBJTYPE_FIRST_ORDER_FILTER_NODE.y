properties:
  Lav_FIRST_ORDER_FILTER_POLE:
    name: pole
    type: float
    range: [-INFINITY, INFINITY]
    default: 0.0
    rate: a
    doc_description: |
      The position of the pole on the real axis.
      
      The pole may be positioned anywhere, but stable filters  usually keep all poles inside the unit circle.
      For a stable filter, the value of this property should usually between -1 and 1.
  Lav_FIRST_ORDER_FILTER_ZERO:
    name: zero
    type: float
    default: 0.0
    range: [-INFINITY, INFINITY]
    rate: a
    doc_description: |
      The position of the zero on the real axis.
inputs:
  - [constructor, "The signal to filter."]
outputs:
  - [constructor, "The filtered signal."]
doc_name: first order filter
doc_description: |
  A first order filter section, implementing the transfer function {{"H(Z) = \frac{B_0 + B_1 Z^{-1} }{1+A_0 Z^{-1} }"|latex}}
  
  This filter is not your friend unless you know DSP or have a specific goal in mind.
  Most applications will want  a {{"Lav_OBJTYPE_BIQUAD_NODE"|node}} or a {{"Lav_OBJTYPE_ONE_POLE_FILTER_NODE"|node}} instead.
  
  This filter is not controlled through frequency specifications.
  Instead, the position of the pole and the zero on the real axis are individually controllable with a-rate properties.
  Some helper functions exist to position them for common configurations, but other filter types do most of it better.
  The major advantage for this filter type is that it is incredibly inexpensive as compared to the {{"Lav_OBJTYPE_IIR_NODE"|node}} and supports automation of the pole and zero's position.