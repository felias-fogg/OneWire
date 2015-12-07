OneWire
=======

This is a modification of the Arduino library for communication using 
the OneWire protocol. It supports also CPU frequencies of 1MHz. It
does so by using sequences of nop instructions for very short delays
instead of delayMicroseconds, which does not support 1MHz in older
Arduino versions. For this reason, it also uses a "correction" factor
of 8 because it assumes 8MHz as the lowest frequency. Since 1.6.5 the
later patch is not neccessary anymore and is disabled.

