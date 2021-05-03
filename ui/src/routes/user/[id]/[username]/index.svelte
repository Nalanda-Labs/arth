<script context="module">
	export function preload({ params }) {
		let username = params.username;
		let id = params.id
		return { username, id };
	}
</script>

<script>
	import { navOptions } from "./_Tabs.svelte";
	import Tab, { Icon, Label } from "@smui/tab";
	import TabBar from "@smui/tab-bar";
	import "../../../_utils.scss";

	export let username;
	export let id;
	let tabName = "Profile";
	let component = navOptions[0].component;

	function getComponent() {
		for(let i=0; i<navOptions.length; i++) {
			if(navOptions[i].page === tabName) {
				return navOptions[i].component;
			}
		}
	}
	function setTabName(name) {
		tabName = name;
		component = getComponent();
	}
</script>

<svelte:head>
	<title>All about {username}</title>
</svelte:head>

<style>
	@media screen and (max-width: 480px) {
		.sm-topic {
			width: 100%;
		}
	}
	@media screen and (min-width:481px) and (max-width: 4098px) {
		.md-topic {
			width: 70%;
		}
	}
</style>

<div class="sm-topic md-topic">
	<TabBar tabs={navOptions.map(option => option.page)} let:tab>
		<Tab {tab} minWidth on:click={setTabName(tab)} style="color: #4285F4">
			<Label>{tab}</Label>
		</Tab>
	</TabBar>

	<svelte:component this={component} {username} {id}/>
</div>
