<script context="module">
	export function preload({ params }) {
		let username = params.username;
		return { username };
	}
</script>

<script>
	import { navOptions } from "./_Tabs.svelte";
	import Tab, { Icon, Label } from "@smui/tab";
	import TabBar from "@smui/tab-bar";
	import "../../_utils.scss";

	export let username;
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
	@media (max-width: 720px) {
		.topic {
			width: 100%;
		}
	}
	@media (max-width: 4096px) {
		.topic {
			width: 800px;
		}
	}
</style>

<div class="topic">
	<!--app navigation -->
	<h3 style="margin-top:20px">{username}'s Profile</h3>
	<hr />
	<TabBar tabs={navOptions.map(option => option.page)} let:tab>
		<Tab {tab} minWidth on:click={setTabName(tab)}>
			<Label>{tab}</Label>
		</Tab>
	</TabBar>

	<svelte:component this={component} {username} />
</div>
