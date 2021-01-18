<script>
    export let taglist;

</script>

<ul style="list-style-type:none">
    {#each taglist as tag}
        <li style="margin-right:5px;background-color:#ddd;display:inline;padding:7px">{tag}</li>
    {/each}
</ul>
