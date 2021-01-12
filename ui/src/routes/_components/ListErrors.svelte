<script>
	export let response;
</script>

{#if response && response.error}
	<ul class="error-messages">
			<li>{response.error}</li>
	</ul>
{/if}