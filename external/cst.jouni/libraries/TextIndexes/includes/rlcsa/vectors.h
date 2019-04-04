#ifndef VECTORS_H
#define VECTORS_H


namespace TextIndexes
{


/*
  These functions merge two vectors using marked positions.
  The original vectors are deleted.
*/

template<class V, class E>
void
handleOne(E& encoder, pair_type& run, usint position)
{
  if(run.second == 0)
  {
    run.first = position;
    run.second = 1;
    return;
  }
  if(position == run.first + run.second)
  {
    run.second++;
    return;
  }
  encoder.setRun(run.first, run.second);
  run.first = position;
  run.second = 1;
}


template<class V, class E>
void
handleRun(E& encoder, pair_type& run, pair_type& next, usint limit)
{
  if(run.second == 0)
  {
    run.first = next.first;
    run.second = std::min(limit - run.first, next.second);
    next.first += run.second;
    next.second -= run.second;
    return;
  }

  if(next.first == run.first + run.second)
  {
    usint cont = std::min(limit - next.first, next.second);
    run.second += cont;
    next.first += cont;
    next.second -= cont;
    return;
  }

  encoder.setRun(run.first, run.second);
  run.first = next.first;
  run.second = std::min(limit - run.first, next.second);;
  next.first += run.second;
  next.second -= run.second;
}


template<class V, class E, class I>
V*
mergeVectors(V* first, V* second, usint* positions, usint n, usint size, usint block_size)
{
  if((first == 0 && second == 0) || positions == 0) { return 0; }

  I* first_iter = 0;
  I* second_iter = 0;

  pair_type first_run;
  bool first_finished;
  if(first == 0)
  {
    first_run = pair_type(size, 0);
    first_finished = true;
  }
  else
  {
    first_iter = new I(*first);
    first_run = first_iter->selectRun(0, size);
    first_run.second++;
    first_finished = false;
  }

  usint second_bit;
  if(second == 0)
  {
    second_bit = n;
  }
  else
  {
    second_iter = new I(*second);
    second_bit = second_iter->select(0);
  }

  E encoder(block_size);
  pair_type run = pair_type(size, 0);
  for(usint i = 0; i < n; i++, first_run.first++)
  {
    while(!first_finished && first_run.first < positions[i])
    {
      handleRun<V, E>(encoder, run, first_run, positions[i]);
      if(first_run.second == 0)
      {
        if(first_iter->hasNext())
        {
          first_run = first_iter->selectNextRun(size);
          first_run.first += i;
          first_run.second++;
        }
        else
        {
          first_finished = true;
        }
      }
    }

    if(i == second_bit) // positions[i] is one
    {
      handleOne<V, E>(encoder, run, positions[i]);
      second_bit = second_iter->selectNext();
    }
    else  // positions[i] is zero
    {
      if(run.second != 0)
      {
        encoder.setRun(run.first, run.second);
        run.second = 0;
      }
    }
  }

  while(!first_finished)
  {
    handleRun<V, E>(encoder, run, first_run, size);
    if(first_iter->hasNext())
    {
      first_run = first_iter->selectNextRun(size);
      first_run.first += n;
      first_run.second++;
    }
    else { break; }
  }

  if(run.second != 0)
  {
    encoder.setRun(run.first, run.second);
  }

  delete first_iter; delete second_iter;
  delete first; delete second;
  return new V(encoder, size);
}


} // namespace TextIndexes


#endif // VECTORS_H
