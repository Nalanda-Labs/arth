<script>
    import { onMount, onDestroy, createEventDispatcher } from "svelte";
  
    export let horizontal = false;
    export let elementScroll;
    export let hasMore = true;
  
    const dispatch = createEventDispatcher();
    let isLoadMore = true;
    let component;
  
    $: {
      if (component || elementScroll) {
        const element = elementScroll ? elementScroll : component.parentNode;
  
        element.addEventListener("scroll", onScroll);
        element.addEventListener("resize", onScroll);
      }
    }
  
    const onScroll = e => {
      const element = e.target;
  
      const offset = horizontal
        ? e.target.scrollWidth - e.target.clientWidth - e.target.scrollLeft
        : e.target.scrollHeight - e.target.clientHeight - e.target.scrollTop;
    };
  
    onDestroy(() => {
      if (component || elementScroll) {
        const element = elementScroll ? elementScroll : component.parentNode;
  
        element.removeEventListener("scroll", null);
        element.removeEventListener("resize", null);
      }
    });
  </script>
  
  <div bind:this={component} style="width:0px" />