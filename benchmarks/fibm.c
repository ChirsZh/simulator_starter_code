/*
 * Redistributions of any form whatsoever must retain and/or include the
 * following acknowledgment, notices and disclaimer:
 *
 * This product includes software developed by Carnegie Mellon University.
 *
 * Copyright (c) 2016 James C. Hoe,
 * Computer Architecture Lab at Carnegie Mellon (CALCM),
 * Carnegie Mellon University.
 *
 * You may not use the name "Carnegie Mellon University" or derivations
 * thereof to endorse or promote products derived from this software.
 *
 * If you modify the software you must place a notice on or within any
 * modified version provided or made available to any third party stating
 * that you have modified the software.  The notice shall include at least
 * your name, address, phone number, email address and the date and purpose
 * of the modification.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER
 * EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO ANYWARRANTY
 * THAT THE SOFTWARE WILL CONFORM TO SPECIFICATIONS OR BE ERROR-FREE AND ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * TITLE, OR NON-INFRINGEMENT.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
 * BE LIABLE FOR ANY DAMAGES, INCLUDING BUT NOT LIMITED TO DIRECT, INDIRECT,
 * SPECIAL OR CONSEQUENTIAL DAMAGES, ARISING OUT OF, RESULTING FROM, OR IN
 * ANY WAY CONNECTED WITH THIS SOFTWARE (WHETHER OR NOT BASED UPON WARRANTY,
 * CONTRACT, TORT OR OTHERWISE).
 */

#define HOW_HIGH 15

int fibm(int n) {
  int *ptr; int i;
  int array[HOW_HIGH+1];

  if (n==0) return 0;
  if (n==1) return 1;

  array[0]=0; array[1]=1;

  for(i=2,ptr=array ; i<=n ; i++,ptr++)
    *(ptr+2)=*(ptr+1)+*ptr;

  return array[n];
}

int main() {
  int count=350;
  int i=0;
  volatile int temp;

  while (count--) {
    if (i<HOW_HIGH) {
      i++;
    } else {
      i=0;
    }

    temp=fibm(i);
  }

  return fibm(HOW_HIGH);
}
