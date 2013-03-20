
/*
Norbert Pozar 2009
http://mekonik.wordpress.com/2009/03/18/arduino-avr-gcc-multiplication/
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

// multiplies a signed long X unsigned int
// stores the high 4 bytes of the result
// rounds the number up if the MSB of the 2 low bytes is set
// 47 cycles
#define MultiSU32X16toH32Round(longRes, longIn1, intIn2) \
asm volatile ( \
"clr r26 \n\t" \
\
\
"mul  %A1, %A2  \n\t" \
"mov  r27, r1   \n\t" \
\
"mul  %B1, %B2  \n\t" \
"movw %A0, r0   \n\t" \
\
"mulsu %D1, %B2 \n\t" \
"movw  %C0, r0  \n\t" \
\
"mulsu %D1, %A2 \n\t" \
"sbc  %D0, r26  \n\t" \
"add  %B0, r0   \n\t" \
"adc  %C0, r1   \n\t" \
"adc  %D0, r26  \n\t" \
\
\
"mul  %B1, %A2  \n\t" \
"add  r27, r0   \n\t" \
"adc  %A0, r1   \n\t" \
"adc  %B0, r26  \n\t" \
"adc  %C0, r26  \n\t" \
"adc  %D0, r26  \n\t" \
\
"mul  %A1, %B2  \n\t" \
"add  r27, r0   \n\t" \
"adc  %A0, r1   \n\t" \
"adc  %B0, r26  \n\t" \
"adc  %C0, r26  \n\t" \
"adc  %D0, r26  \n\t" \
\
"mul  %C1, %A2  \n\t" \
"adc  %A0, r0   \n\t" \
"adc  %B0, r1   \n\t" \
"adc  %C0, r26  \n\t" \
"adc  %D0, r26  \n\t" \
\
"mul  %C1, %B2  \n\t" \
"adc  %B0, r0   \n\t" \
"adc  %C0, r1   \n\t" \
"adc  %D0, r26  \n\t" \
\
\
"lsl  r27       \n\t" \
"adc  %A0, r26  \n\t" \
"adc  %B0, r26  \n\t" \
"adc  %C0, r26  \n\t" \
"adc  %D0, r26  \n\t" \
\
\
"clr r1 \n\t" \
: \
"=&r" (longRes) \
: \
"a" (longIn1), \
"a" (intIn2) \
: \
"r26","r27" \
)
